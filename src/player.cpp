#include "player.h"
#include <vector>
#define GRAVITY 400

Player::Player(Vector2 position, float speed, bool canJump) {
  this->position = position;
  this->speed = speed;
  this->canJump = canJump;
}

void Player::updatePlayer(float delta, std::vector<EnvItem> &envItems) {
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    position.x -= PLAYER_MOVE_SPEED * delta;
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    position.x += PLAYER_MOVE_SPEED * delta;
  if ((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) &&
      canJump) {
    speed = -PLAYER_JUMP_SPEED;
    canJump = false;
  }

  bool hitObstacle = false;

  for (auto &envItem : envItems) {
    if (envItem.blocking && envItem.rect.x <= position.x &&
        envItem.rect.x + envItem.rect.width >= position.x &&
        envItem.rect.y >= position.y &&
        envItem.rect.y <= position.y + (speed * delta)) {
      hitObstacle = true;
      speed = 0.0f;
      position.y = envItem.rect.y;
      break;
    }
  }

  if (!hitObstacle) {
    position.y += speed * delta;
    speed += GRAVITY * delta;
    canJump = false;
  } else
    canJump = true;
}

void Player::draw() {
    // Dibujo simple del jugador como un rectángulo (ajústalo si quieres sprite)
    Rectangle r = bounds();
    DrawRectangleRec(r, (Color){66, 135, 245, 255});
    DrawRectangleLinesEx(r, 2, BLACK);
}
