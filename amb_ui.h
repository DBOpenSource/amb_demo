#pragma once

namespace amb {

enum class Command : char {
  kColorMusic   = '1',
  kColorTts     = '2',
  kColorTtsPico = '3',
  kVoiceAws     = '4',
  kVoiceTest    = '5',
  kStart        = 'a',
  kStop         = 'b',
  kExit         = 'x',
  kInvalid      = '0'
};

class Menu {
 public:
  Menu() {};
  ~Menu() {};

  Command GetCommand();
  void PrintMenu();

 private:
  Menu(const Menu&) = delete;
  Menu(Menu&&) = delete;
  Menu& operator=(const Menu&) = delete;
  Menu& operator=(const Menu&&) = delete;
};

};
