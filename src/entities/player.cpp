#include "player.h"
#include "../core/textureManager.h"
#include <vector>
#define GRAVITY 600

Player::Player(Vector2 position, float speed, bool canJump) {
  this->position = position;
  this->speed = speed;
  this->canJump = canJump;

  // Intentar cargar textura (el nombre "player" se asocia en main o aquí)
  // Por ahora asumimos que se cargará externamente o usamos el getter
  this->texture = TextureManager::Instance().Get("player");
  
  // Si la textura es válida, configuramos frameRec
  if (this->texture.id != 0) {
      // Heurística: si ancho > 2 * alto, asumimos strip de 4 frames. Si no, 1 frame.
      bool isStrip = (this->texture.width > this->texture.height * 2);
      int numFrames = isStrip ? 4 : 1;
      
      float frameWidth = (float)this->texture.width / (float)numFrames; 
      this->frameRec = { 0.0f, 0.0f, frameWidth, (float)this->texture.height };
  }
}

void Player::updatePlayer(float delta, std::vector<EnvItem> &envItems) {
  // Movimiento horizontal
  float deltaX = 0;
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
    deltaX -= PLAYER_MOVE_SPEED * delta;
    isFacingRight = false;
  }
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
    deltaX += PLAYER_MOVE_SPEED * delta;
    isFacingRight = true;
  }

  position.x += deltaX;

  // Colisión horizontal
  Rectangle playerRect = {position.x, position.y - height, width, height};
  for (auto &envItem : envItems) {
    if (envItem.blocking && CheckCollisionRecs(playerRect, envItem.rect)) {
      if (deltaX > 0) {
        // Moviendo a la derecha, colocar al jugador justo a la izquierda del obstáculo
        position.x = envItem.rect.x - width;
      } else if (deltaX < 0) {
        // Moviendo a la izquierda, colocar al jugador justo a la derecha del obstáculo
        position.x = envItem.rect.x + envItem.rect.width;
      }
    }
  }

  // Salto
  if ((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) &&
      canJump) {
    speed = -PLAYER_JUMP_SPEED;
    canJump = false;
  }

  // Colisión vertical
  bool hitObstacle = false;

  for (auto &envItem : envItems) {
    if (envItem.blocking &&
        envItem.rect.x <= position.x + width &&
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

  // Animation Update
  if (texture.id != 0) {
      // Solo animar si el frameRec es menor que el texture width (hay más de 1 frame)
      if (frameRec.width < texture.width) {
          framesCounter++;
          if (framesCounter >= (60/framesSpeed)) {
              framesCounter = 0;
              currentFrame++;
              // Asumimos 4 frames si animamos
              if (currentFrame > 3) currentFrame = 0;
              frameRec.x = (float)currentFrame * frameRec.width;
          }
          
          // Si no se mueve, frame 0
          if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
              currentFrame = 0;
              frameRec.x = 0;
          }
      }
  }
}

void Player::draw() {
  // Refrescar textura por si se cargó tarde
  if (texture.id == 0) {
      texture = TextureManager::Instance().Get("player");
      if (texture.id != 0) {
           bool isStrip = (this->texture.width > this->texture.height * 2);
           int numFrames = isStrip ? 4 : 1;
           float frameWidth = (float)this->texture.width / (float)numFrames; 
           this->frameRec = { 0.0f, 0.0f, frameWidth, (float)this->texture.height };
      }
  }

  if (texture.id != 0) {
      Rectangle destRec = { position.x, position.y - height, width, height }; // Ajuste por pivote en pies
      // Invertir frameRec width si mira a la izquierda
      Rectangle source = frameRec;
      if (!isFacingRight) source.width = -source.width;
      
      DrawTexturePro(texture, source, destRec, {0,0}, 0.0f, WHITE);
  } else {
      // Fallback
      Rectangle r = bounds();
      DrawRectangleRec(r, (Color){66, 135, 245, 255});
      DrawRectangleLinesEx(r, 2, BLACK);
  }
}
