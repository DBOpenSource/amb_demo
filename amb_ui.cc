#include "amb_ui.h"

#include <iostream>
#include <type_traits>

namespace amb {

using ::std::cin;
using ::std::cout;
using ::std::endl;
using ::std::ostream;
using ::std::underlying_type;

ostream& operator<<(ostream& stream, Command obj)
{
  return stream << static_cast<underlying_type<Command>::type>(obj);
}

void Menu::PrintMenu() {
  cout << endl << endl;
  cout << "====== AMB AUDIO DEMO MENU ======" << endl;

  cout << Command::kColorMusic << ". Select Color to Music Demo" << endl;
  cout << Command::kColorTts << ". Select Color to Local TTS Demo" << endl;
  cout << Command::kColorTtsPico << ". Select Color to Pico TTS Demo" << endl;
  cout << Command::kVoiceAws << ". Select AWS Voice Recognition Demo" << endl;
  cout << Command::kVoiceTest << ". Select Voice Test Demo" << endl;
  cout << Command::kStart << ". Start Demo" << endl;
  cout << Command::kStop << ". Stop Demo" << endl;
  cout << Command::kExit << ". Exit Demo" << endl;

  cout << endl;
  cout << "Selection: ";
}

Command Menu::GetCommand() {
  PrintMenu();

  char selection;
  cin >> selection;
  return Command(static_cast<Command>(selection));
}

};
