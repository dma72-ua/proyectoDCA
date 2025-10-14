#include "player.h"
#include <vector>

Player::Player(Vector2 position, float speed, bool canJump) {
  this->position = position;
  this->speed = speed;
  this->canJump = canJump;
}

void Player::updatePlayer(float delta, std::vector<EnvItem> &envItems) {
  if (IsKeyDown(KEY_LEFT))
    position.x -= PLAYER_MOVE_SPEED * delta;
  if (IsKeyDown(KEY_RIGHT))
    position.x += PLAYER_MOVE_SPEED * delta;
  if (IsKeyDown(KEY_SPACE) && canJump) {
    speed = -PLAYER_JUMP_SPEED;
    canJump = false;
  }

  bool hitObstacle = false;

  for (auto &envItem : envItems) {
    if (envItem.blocking && envItem.rect.x + envItem.rect.width >= position.x &&
        envItem.rect.y >= position.y &&
        envItem.rect.y <= position.y + speed * delta) {
      hitObstacle = true;
      speed = 0.0f;
      position.y = envItem.rect.y;
      break;
    }
  }
}
