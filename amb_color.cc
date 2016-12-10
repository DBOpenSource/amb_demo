#include "amb_color.h"

#include <chrono>
#include <mutex>
#include <queue>
#include <thread>

#include "amb_grove.h"

namespace amb {

using ::std::chrono::seconds;
using ::std::mutex;
using ::std::queue;
using ::std::this_thread::sleep_for;
using ::std::thread;

static const int kSamplePeriod = 2;

ColorSensor::ColorSensor(const ErrorHandlerType& error_handler,
                         const ColorHandlerType& color_handler)
    : error_handler_(error_handler),
      color_handler_(color_handler),
      thread_(nullptr) {
}

ColorSensor::~ColorSensor() {
  Disable();
}

int ColorSensor::Enable() {
  if (thread_ != nullptr)
    return -EINPROGRESS;

  thread_ = new thread(ColorSensor::StaticThreadEntry, this);
  if (thread_ == nullptr)
    return -ECHILD;

  return 0;
}

void ColorSensor::Disable() {
  if (thread_ == nullptr)
    return;

  Message message;
  message.type = MessageType::kDisable;

  message_lock_.lock();
  messages_.push(message);
  message_lock_.unlock();

  thread_->join();
  delete thread_;
  thread_ = nullptr;
}

void ColorSensor::StaticThreadEntry(ColorSensor* color_sensor) {
  color_sensor->ThreadColorSensor();
}

void ColorSensor::ThreadColorSensor() {
  GroveColorSensor color_sensor;

  while (!messages_.empty())
    messages_.pop();

  color_sensor.Init();
  color_sensor.ledStatus = 1;

  bool keep_running = true;
  while (keep_running) {
    message_lock_.lock();
    if (!messages_.empty()) {
      Message message = messages_.front();

      switch (message.type) {
        case MessageType::kDisable:
          keep_running = false;
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
      int red, green, blue;

      color_sensor.readRGB(&red, &green, &blue);

      Color color = Color::kNone;
      if (red > 200 && green > 100 && blue < 100)
        color = Color::kYellow;
      else if (red > 200 && green < 100 && blue < 100)
        color = Color::kRed;
      else if (green > 100 && blue < 100)
        color = Color::kGreen;
      else if (blue > 100 && red < 100)
        color = Color::kBlue;
      color_handler_(color);

      sleep_for(seconds(kSamplePeriod));
      color_sensor.clearInterrupt();
    }
  }

  color_sensor.ledStatus = 0;
  color_sensor.Deinit();
}

};
