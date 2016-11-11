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

class Playback
{
 public:
  typedef function<void(const int error)> ErrorHandlerType;
  typedef function<void(const Buffer& buffer)> BufferHandlerType;

  Playback(const ErrorHandlerType& error_handler,
           const BufferHandlerType& empty_buffer_handler);
  ~Playback();

  int Open(const Metadata& metadata);
  void Close();

  int Start();
  void Stop();

  int WriteBuffer(const Buffer& buffer);

 private:
  enum class MessageType {
    kStop,
    kBuffer,
  };

  struct Message {
    MessageType type;
    Buffer buffer;
  };

  static void StaticThreadEntry(Playback* playback);
  void ThreadPlayback();

  ErrorHandlerType error_handler_;
  BufferHandlerType empty_buffer_handler_;

  pa_simple* pulseaudio_handle_;

  thread* thread_;
  mutex message_lock_;
  queue<Message> messages_;
  condition_variable signal_;

  Playback(const Playback&) = delete;
  Playback(Playback&&) = delete;
  Playback& operator=(const Playback&) = delete;
  Playback& operator=(const Playback&&) = delete;
};

};
