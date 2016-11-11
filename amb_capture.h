#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include <pulse/simple.h>

#include "amb_buffers.h"

namespace amb {

using ::std::condition_variable;
using ::std::function;
using ::std::mutex;
using ::std::queue;
using ::std::thread;

class Capture
{
 public:
  typedef function<void(const int error)> ErrorHandlerType;
  typedef function<void(const Buffer& buffer)> BufferHandlerType;

  Capture(const ErrorHandlerType& error_handler,
          const BufferHandlerType& full_buffer_handler);
  ~Capture();

  int Open(const Metadata& metadata);
  void Close();

  int Start();
  void Stop();

  int ReturnBuffer(const Buffer& buffer);

 private:
  enum class MessageType {
    kStop,
    kBuffer,
  };

  struct Message {
    MessageType type;
    Buffer buffer;
  };

  static void StaticThreadEntry(Capture* capture);
  void ThreadCapture();

  ErrorHandlerType error_handler_;
  BufferHandlerType full_buffer_handler_;

  pa_simple* pulseaudio_handle_;
  Buffers buffers_;

  thread* thread_;
  mutex message_lock_;
  queue<Message> messages_;
  condition_variable signal_;

  Capture(const Capture&) = delete;
  Capture(Capture&&) = delete;
  Capture& operator=(const Capture&) = delete;
  Capture& operator=(const Capture&&) = delete;
};

};
