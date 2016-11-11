#include "amb_capture.h"

#include <cerrno>
#include <cstring>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include <pulse/simple.h>
#include <pulse/error.h>

#include "amb_buffers.h"

namespace amb {

using ::std::condition_variable;
using ::std::mutex;
using ::std::queue;
using ::std::thread;
using ::std::unique_lock;

static const char* kClientName = "AMB";
static const char* kStreamName = "capture";

static const int kBufferNumber = 4;
static const int kBufferCapacity = 4 * 1024;

Capture::Capture(const ErrorHandlerType& error_handler,
                 const BufferHandlerType& full_buffer_handler)
    : error_handler_(error_handler),
      full_buffer_handler_(full_buffer_handler),
      pulseaudio_handle_(nullptr),
      thread_(nullptr) {
}

Capture::~Capture() {
  Stop();
  Close();
}

int Capture::Open(const Metadata& metadata) {
  pa_sample_spec pulseaudio_spec;
  switch (metadata.sample_size) {
    case 16: pulseaudio_spec.format = PA_SAMPLE_S16LE; break;
    case 24: pulseaudio_spec.format = PA_SAMPLE_S24LE; break;
    case 32: pulseaudio_spec.format = PA_SAMPLE_S32LE; break;
    default:
      return -EINVAL;
      break;
  }
  pulseaudio_spec.rate = metadata.sample_rate;
  pulseaudio_spec.channels = metadata.num_channels;

  int error;
  pulseaudio_handle_ = pa_simple_new(NULL, kClientName, PA_STREAM_RECORD, NULL,
                                     kStreamName, &pulseaudio_spec, NULL, NULL,
                                     &error);
  if (pulseaudio_handle_ == nullptr)
    return -EBADF;

  return 0;
}

void Capture::Close() {
  if (pulseaudio_handle_ != nullptr)
    pa_simple_free(pulseaudio_handle_);
  pulseaudio_handle_ = nullptr;
}

int Capture::Start() {
  if (thread_ != nullptr)
    return -EINPROGRESS;

  thread_ = new thread(Capture::StaticThreadEntry, this);
  if (thread_ == nullptr)
    return -ECHILD;

  return 0;
}

void Capture::Stop() {
  if (thread_ == nullptr)
    return;

  Message message;
  message.type = MessageType::kStop;

  message_lock_.lock();
  messages_.push(message);
  message_lock_.unlock();
  signal_.notify_one();

  thread_->join();
  delete thread_;
  thread_ = nullptr;
}

int Capture::ReturnBuffer(const Buffer& buffer) {
  Message message;
  message.type = MessageType::kBuffer;
  message.buffer = buffer;

  message_lock_.lock();
  messages_.push(message);
  message_lock_.unlock();
  signal_.notify_one();

  return 0;
}

void Capture::StaticThreadEntry(Capture* capture) {
  capture->ThreadCapture();
}

void Capture::ThreadCapture() {
  queue<Buffer> buffers;
  int result, error;

  while (!messages_.empty())
    messages_.pop();

  result = buffers_.Allocate(kBufferNumber, kBufferCapacity, &buffers);
  if (result < 0) {
    error_handler_(result);
    return;
  }

  bool keep_running = true;
  while (keep_running) {
    if (messages_.empty() && buffers.empty()) {
      unique_lock<mutex> lk(message_lock_);
      signal_.wait(lk);
    }

    message_lock_.lock();
    if (!messages_.empty()) {
      Message message = messages_.front();

      switch (message.type) {
        case MessageType::kStop:
          keep_running = false;
          break;

        case MessageType::kBuffer:
          buffers.push(message.buffer);
          break;

        default:
          error_handler_(-EINVAL);
          keep_running = false;
          break;
      }
      messages_.pop();
    }
    message_lock_.unlock();

    if (keep_running) {
      while (!buffers.empty()) {
        Buffer buffer = buffers.front();

        result = pa_simple_read(pulseaudio_handle_, buffer.data,
                                buffer.capacity, &error);
        if (result < 0) {
          error_handler_(-EIO);
          keep_running = false;
          break;
        }

        buffer.size = buffer.capacity;
        full_buffer_handler_(buffer);

        buffers.pop();
      }
    }
  }

  buffers_.Deallocate();
}

};
