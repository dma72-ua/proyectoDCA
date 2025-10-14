#pragma once

extern "C" {
#include "../vendor/include/raylib.h"
}

class EnvItem {
public:
  EnvItem(Rectangle rect, int blocking, Color color) {
    this->rect = rect;
    this->blocking = blocking;
    this->color = color;
  }
  Rectangle rect;
  int blocking;
  Color color;

private:
};
