#pragma once
#include "envItem.h"
#include <vector>

extern "C" {
#include "../vendor/include/raylib.h"
}

class Player {
public:
  Player(Vector2, float, bool);
  void updatePlayer(float, std::vector<EnvItem> &);
  Vector2 position;

private:
  float speed;
  bool canJump;

  float PLAYER_MOVE_SPEED = 180.0f;
  float PLAYER_JUMP_SPEED = 250.0f;
};
