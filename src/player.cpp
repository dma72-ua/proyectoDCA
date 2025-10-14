#include "player.h"

Player::Player(Vector2 position, float speed, bool canJump) {
  this->position = position;
  this->speed = speed;
  this->canJump = canJump;
}

void Player::updatePlayer(float delta) {
  if (IsKeyDown(KEY_LEFT))
    position.x -= PLAYER_MOVE_SPEED * delta;
  if (IsKeyDown(KEY_RIGHT))
    position.x += PLAYER_MOVE_SPEED * delta;
  if (IsKeyDown(KEY_SPACE) && canJump) {
    speed = -PLAYER_JUMP_SPEED;
    canJump = false;
  }
}
