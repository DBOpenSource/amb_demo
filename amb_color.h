#pragma once

#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "amb_grove.h"

namespace amb {

using ::std::function;
using ::std::mutex;
using ::std::queue;
using ::std::thread;

enum class Color {
  kBlue,
  kGreen,
  kRed,
  kYellow,
  kNone
};

class ColorSensor
{
 public:
  typedef function<void(const int error)> ErrorHandlerType;
  typedef function<void(const Color color)> ColorHandlerType;

  ColorSensor(const ErrorHandlerType& error_handler,
              const ColorHandlerType& color_handler);
  ~ColorSensor();

  int Enable();
  void Disable();

 private:
  enum class MessageType {
    kDisable,
  };

  struct Message {
    MessageType type;
  };

  static void StaticThreadEntry(ColorSensor* color_sensor);
  void ThreadColorSensor();

  ErrorHandlerType error_handler_;
  ColorHandlerType color_handler_;

  thread* thread_;
  mutex message_lock_;
  queue<Message> messages_;

  ColorSensor(const ColorSensor&) = delete;
  ColorSensor(ColorSensor&&) = delete;
  ColorSensor& operator=(const ColorSensor&) = delete;
  ColorSensor& operator=(const ColorSensor&&) = delete;
};

};
