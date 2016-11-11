#include "amb_demo.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "amb_buffers.h"
#include "amb_capture.h"
#include "amb_color.h"
#include "amb_playback.h"

namespace amb {

using ::std::condition_variable;
using ::std::cerr;
using ::std::endl;
using ::std::mutex;
using ::std::queue;
using ::std::string;
using ::std::system;
using ::std::thread;
using ::std::unique_lock;

Demo::Demo()
    : use_case_(UseCase::kNone), capture_(nullptr), playback_(nullptr),
      color_sensor_(nullptr), thread_(nullptr) {
}

Demo::~Demo() {
}

void Demo::ColorMusic() {
  use_case_ = UseCase::kColorMusic;
}

void Demo::ColorTts() {
  use_case_ = UseCase::kColorTts;
}

void Demo::ColorTtsPico() {
  use_case_ = UseCase::kColorTtsPico;
}

void Demo::VoiceAws() {
  use_case_ = UseCase::kVoiceAws;
}

void Demo::VoiceTest() {
  use_case_ = UseCase::kVoiceTest;
}

void Demo::Start() {
  if (thread_ != nullptr) {
    cerr << "WARNING: Cannot invoke Start command with a running demo."
         << endl;
    return;
  }

  thread_ = new thread(Demo::StaticThreadEntry, this);
  if (thread_ == nullptr) {
    cerr << "ERROR: instantiation failure.  Aborting." << endl;
    assert(false);
  }
}

void Demo::Stop() {
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

void Demo::ErrorHandler(const int error) {
  cerr << "ERROR: Running demo encounter error.  Aborting." << endl;
  assert(false);
}

void Demo::FullBufferHandler(const Buffer& buffer) {
  Message message;
  message.type = MessageType::kFullBuffer;
  message.buffer = buffer;

  message_lock_.lock();
  messages_.push(message);
  message_lock_.unlock();
  signal_.notify_one();
}

void Demo::EmptyBufferHandler(const Buffer& buffer) {
  Message message;
  message.type = MessageType::kEmptyBuffer;
  message.buffer = buffer;

  message_lock_.lock();
  messages_.push(message);
  message_lock_.unlock();
  signal_.notify_one();
}

void Demo::ColorHandler(const Color color) {
  Message message;
  message.type = MessageType::kColor;
  message.color = color;

  message_lock_.lock();
  messages_.push(message);
  message_lock_.unlock();
  signal_.notify_one();
}

void Demo::StaticThreadEntry(Demo* demo) {
  demo->ThreadEntry();
}

void Demo::ThreadEntry() {
  switch (use_case_) {
    case UseCase::kColorMusic:   ThreadColorMusic();   break;
    case UseCase::kColorTts:     ThreadColorTts();     break;
    case UseCase::kColorTtsPico: ThreadColorTtsPico(); break;
    case UseCase::kVoiceAws:     ThreadVoiceAws();     break;
    case UseCase::kVoiceTest:    ThreadVoiceTest();    break;
    default:
      cerr << "WARNING: No demo has been selected." << endl;
      break;
  }
}

void Demo::ThreadColorMusic() {
  Color current_color = Color::kNone;
  Color pending_color = Color::kNone;
  bool keep_running = true;

  while (!messages_.empty())
    messages_.pop();

  if (color_sensor_ == nullptr) {
    ColorSensor::ErrorHandlerType error_handler =
      [this](const int error) -> void {
        ErrorHandler(error);
      };

    ColorSensor::ColorHandlerType color_handler =
      [this](const Color color) -> void {
        ColorHandler(color);
      };

    color_sensor_ = new ColorSensor(error_handler, color_handler);
    if (color_sensor_ == nullptr) {
      ErrorHandler(-ENOMEM);
      goto on_error;
    }
  }
  color_sensor_->Enable();

  while (keep_running) {
    if (messages_.empty()) {
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

        case MessageType::kColor:
          pending_color = message.color;
          break;

        default:
          ErrorHandler(-EINVAL);
          keep_running = false;
          break;
      }
      messages_.pop();
    }
    message_lock_.unlock();

    if (keep_running &&
        current_color != pending_color &&
        pending_color != Color::kNone) {
      system("kill -9 $(ps -e -o 'pid,args' | grep vlc | grep -v grep | cut -c'-5')");

      string command = "cvlc ";
      switch (pending_color) {
        case Color::kBlue:   command.append("songs/Blue.mp3 &");   break;
        case Color::kGreen:  command.append("songs/Green.mp3 &");  break;
        case Color::kRed:    command.append("songs/Red.mp3 &");    break;
        case Color::kYellow: command.append("songs/Yellow.mp3 &"); break;
        default:
          ErrorHandler(-EINVAL);
          keep_running = false;
          break;
      }
      system(command.c_str());

      current_color = pending_color;
    }
  }

on_error:
  system("kill -9 $(ps -e -o 'pid,args' | grep vlc | grep -v grep | cut -c'-5')");

  color_sensor_->Disable();
  delete color_sensor_;
  color_sensor_ = nullptr;
}

void Demo::ThreadColorTts() {
  Color current_color = Color::kNone;
  Color pending_color = Color::kNone;
  bool keep_running = true;

  while (!messages_.empty())
    messages_.pop();

  if (color_sensor_ == nullptr) {
    ColorSensor::ErrorHandlerType error_handler =
      [this](const int error) -> void {
        ErrorHandler(error);
      };

    ColorSensor::ColorHandlerType color_handler =
      [this](const Color color) -> void {
        ColorHandler(color);
      };

    color_sensor_ = new ColorSensor(error_handler, color_handler);
    if (color_sensor_ == nullptr) {
      ErrorHandler(-ENOMEM);
      goto on_error;
    }
  }
  color_sensor_->Enable();

  while (keep_running) {
    if (messages_.empty()) {
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

        case MessageType::kColor:
          pending_color = message.color;
          break;

        default:
          ErrorHandler(-EINVAL);
          keep_running = false;
          break;
      }
      messages_.pop();
    }
    message_lock_.unlock();

    if (keep_running &&
        current_color != pending_color &&
        pending_color != Color::kNone) {
      string command = "espeak -g8 -k12 -p45 -s130 \"the color is ";
      switch (pending_color) {
        case Color::kBlue:   command.append("Blue\"");  break;
        case Color::kGreen:  command.append("Green\"");  break;
        case Color::kRed:    command.append("Red\"");    break;
        case Color::kYellow: command.append("Yellow\""); break;
        default:
          ErrorHandler(-EINVAL);
          keep_running = false;
          break;
      }
      system(command.c_str());

      current_color = pending_color;
    }
  }

on_error:
  color_sensor_->Disable();
  delete color_sensor_;
  color_sensor_ = nullptr;
}

void Demo::ThreadColorTtsPico() {
  Color current_color = Color::kNone;
  Color pending_color = Color::kNone;
  bool keep_running = true;

  while (!messages_.empty())
    messages_.pop();

  if (color_sensor_ == nullptr) {
    ColorSensor::ErrorHandlerType error_handler =
      [this](const int error) -> void {
        ErrorHandler(error);
      };

    ColorSensor::ColorHandlerType color_handler =
      [this](const Color color) -> void {
        ColorHandler(color);
      };

    color_sensor_ = new ColorSensor(error_handler, color_handler);
    if (color_sensor_ == nullptr) {
      ErrorHandler(-ENOMEM);
      goto on_error;
    }
  }
  color_sensor_->Enable();

  while (keep_running) {
    if (messages_.empty()) {
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

        case MessageType::kColor:
          pending_color = message.color;
          break;

        default:
          ErrorHandler(-EINVAL);
          keep_running = false;
          break;
      }
      messages_.pop();
    }
    message_lock_.unlock();

    if (keep_running &&
        current_color != pending_color &&
        pending_color != Color::kNone) {
      system("kill -9 $(ps -e -o 'pid,args' | grep vlc | grep -v grep | cut -c'-5')");

      string command = "pico2wave -w /var/local/output.wav \"the color is ";
      switch (pending_color) {
        case Color::kBlue:   command.append("blue\"");   break;
        case Color::kGreen:  command.append("green\"");  break;
        case Color::kRed:    command.append("red\"");    break;
        case Color::kYellow: command.append("yellow\""); break;
        default:
          ErrorHandler(-EINVAL);
          keep_running = false;
          break;
      }
      system(command.c_str());
      system("cvlc /var/local/output.wav &");

      current_color = pending_color;
    }
  }

on_error:
  system("kill -9 $(ps -e -o 'pid,args' | grep vlc | grep -v grep | cut -c'-5')");

  color_sensor_->Disable();
  delete color_sensor_;
  color_sensor_ = nullptr;
}

void Demo::ThreadVoiceAws() {
  system("cd ../alexa/alexa-avs-raspberry-pi/samples/javaclient && mvn exec:exec");
}

void Demo::ThreadVoiceTest() {
  queue<Buffer> full_buffers;
  queue<Buffer> empty_buffers;
  bool keep_running = true;
  int result;

  Metadata metadata = {
    1,
    44100,
    16,
  };

  while (!messages_.empty())
    messages_.pop();

  if (capture_ == nullptr) {
    Capture::ErrorHandlerType error_handler =
      [this](const int error) -> void {
        ErrorHandler(error);
      };

    Capture::BufferHandlerType full_buffer_handler =
      [this](const Buffer& buffer) -> void {
        FullBufferHandler(buffer);
      };

    capture_ = new Capture(error_handler, full_buffer_handler);
    if (capture_ == nullptr) {
      ErrorHandler(-ENOMEM);
      goto on_error;
    }
  }

  if (playback_ == nullptr) {
    Playback::ErrorHandlerType error_handler =
      [this](const int error) -> void {
        ErrorHandler(error);
      };

    Playback::BufferHandlerType empty_buffer_handler =
      [this](const Buffer& buffer) -> void {
        EmptyBufferHandler(buffer);
      };

    playback_ = new Playback(error_handler, empty_buffer_handler);
    if (playback_ == nullptr) {
      ErrorHandler(-ENOMEM);
      goto on_error;
    }
  }

  result = capture_->Open(metadata);
  if (result < 0) {
    ErrorHandler(result);
    goto on_error;
  }
  result = playback_->Open(metadata);
  if (result < 0) {
    ErrorHandler(result);
    goto on_error;
  }

  result = playback_->Start();
  if (result < 0) {
    ErrorHandler(result);
    goto on_error;
  }
  result = capture_->Start();
  if (result < 0) {
    ErrorHandler(result);
    goto on_error;
  }

  while (keep_running) {
    if (messages_.empty() && full_buffers.empty() && empty_buffers.empty()) {
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

        case MessageType::kFullBuffer:
          full_buffers.push(message.buffer);
          break;

        case MessageType::kEmptyBuffer:
          empty_buffers.push(message.buffer);
          break;

        case MessageType::kColor:
        default:
          ErrorHandler(-EINVAL);
          keep_running = false;
          break;
      }
      messages_.pop();
    }
    message_lock_.unlock();

    if (keep_running) {
      while (!full_buffers.empty()) {
        Buffer buffer = full_buffers.front();

        result = playback_->WriteBuffer(buffer);
        if (result < 0) {
          ErrorHandler(result);
          keep_running = false;
        }

        full_buffers.pop();
      }
    }

    if (keep_running) {
      while (!empty_buffers.empty()) {
        Buffer buffer = empty_buffers.front();

        result = capture_->ReturnBuffer(buffer);
        if (result < 0) {
          ErrorHandler(result);
          keep_running = false;
        }

        empty_buffers.pop();
      }
    }
  }

on_error:
  if (capture_ != nullptr) {
    capture_->Stop();
    capture_->Close();
    delete capture_;
    capture_ = nullptr;
  }

  if (playback_ != nullptr) {
    playback_->Stop();
    playback_->Close();
    delete playback_;
    playback_ = nullptr;
  }
}

};
