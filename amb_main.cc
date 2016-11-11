#include <iostream>

#include "amb_demo.h"
#include "amb_ui.h"

using ::amb::Command;
using ::amb::Menu;
using ::amb::Demo;
using ::std::cout;
using ::std::endl;

int main(const int argc, const char * const argv[]) {
  system("cd ../alexa/alexa-avs-raspberry-pi/samples/companionService && npm start &");
  system("cd ../alexa/alexa-avs-raspberry-pi/samples/javaclient && mvn exec:exec");

  Demo demo;
  Menu menu;
  bool keep_running = true;
  while (keep_running) {
    Command command = menu.GetCommand();

    switch(command) {
      case Command::kColorMusic:
        demo.ColorMusic();
        break;
      case Command::kColorTts:
        demo.ColorTts();
        break;
      case Command::kColorTtsPico:
        demo.ColorTtsPico();
        break;
      case Command::kVoiceAws:
        demo.VoiceAws();
        break;
      case Command::kVoiceTest:
        demo.VoiceTest();
        break;
      case Command::kStart:
        demo.Start();
        break;
      case Command::kStop:
        demo.Stop();
        break;
      case Command::kExit:
        demo.Stop();
        keep_running = false;
        break;
      default:
        cout << endl;
        cout << "Invalid selection" << endl;
        break;
    }
  }

  system("kill -9 $(ps -e -o 'pid,args' | grep npm | grep -v grep | cut -c'-5')");
  system("kill -9 $(ps -e -o 'pid,args' | grep 'sh -c node' | grep -v grep | cut -c'-5')");
  system("kill -9 $(ps -e -o 'pid,args' | grep node | grep -v grep | cut -c'-5')");

  return 0;
}
