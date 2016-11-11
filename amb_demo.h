#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include "amb_buffers.h"
#include "amb_capture.h"
#include "amb_color.h"
#include "amb_playback.h"

namespace amb {

using ::std::condition_variable;
using ::std::mutex;
using ::std::queue;
using ::std::thread;

class Demo
{
 public:
  Demo();
  ~Demo();

  void ColorMusic();
  void ColorTts();
  void ColorTtsPico();
  void VoiceAws();
  void VoiceTest();

  void Start();
  void Stop();

 private:
  enum class UseCase {
    kColorMusic,
    kColorTts,
    kColorTtsPico,
    kVoiceAws,
    kVoiceTest,
    kNone
  };

  enum class MessageType {
    kStop,
    kFullBuffer,
    kEmptyBuffer,
    kColor,
  };

  struct Message {
    MessageType type;
    union {
      Buffer buffer;
      Color color;
    };
  };

  void ErrorHandler(const int error);
  void FullBufferHandler(const Buffer& buffer);
  void EmptyBufferHandler(const Buffer& buffer);
  void ColorHandler(const Color color);

  static void StaticThreadEntry(Demo* demo);
  void ThreadEntry();
  void ThreadColorMusic();
  void ThreadColorTts();
  void ThreadColorTtsPico();
  void ThreadVoiceAws();
  void ThreadVoiceTest();

  UseCase use_case_;

  Capture* capture_;
  Playback* playback_;
  ColorSensor* color_sensor_;

  thread* thread_;
  mutex message_lock_;
  queue<Message> messages_;
  condition_variable signal_;

  Demo(const Demo&) = delete;
  Demo(Demo&&) = delete;
  Demo& operator=(const Demo&) = delete;
  Demo& operator=(const Demo&&) = delete;
};

};
