#pragma once

extern "C" {
#include "../vendor/include/raylib.h"
}

class EnvItem {
public:
  EnvItem(Rectangle rect, int blocking, Color color, int textureId = 0) {
    this->rect = rect;
    this->blocking = blocking;
    this->color = color;
    this->textureId = textureId;
  }
  Rectangle rect;
  int blocking;
  Color color;
  // Modeling support
  int textureId = 0; // 0 = default/color, 1 = bricks, 2 = pipe, etc.

private:
};
