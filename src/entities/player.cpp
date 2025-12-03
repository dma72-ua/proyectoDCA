#include "player.h"
#include "../core/textureManager.h"
#include <vector>
#include <cmath>
#define GRAVITY 600

Player::Player(Vector2 position, float speed, bool canJump) {
  this->position = position;
  this->speed = speed;
  this->canJump = canJump;

  // Intentar cargar textura
  this->texture = TextureManager::Instance().Get("player");
  
  // Si la textura es válida, configuramos frameRec
  if (this->texture.id != 0) {
      totalFrames = 7; // 7 columnas
      
      // Spritesheet: 350x407 pixels
      // 7 columnas = 50 pixels de ancho por frame
      // 11 filas = 37 pixels de alto por frame
      float frameWidth = 50.0f;   // 350 / 7
      float frameHeight = 37.0f;  // 407 / 11
      
      this->frameRec = { 0.0f, 0.0f, frameWidth, frameHeight };
      
      // Ajustar hitbox del jugador (más pequeña que el sprite visual)
      this->width = 30.0f;
      this->height = 35.0f;
  }
  
  // Inicializar animación
  currentFrame = 0;
  frameTime = 0.0f;
  frameSpeed = 10.0f;
  currentAnim = IDLE;
  previousAnim = IDLE;
  lastHorizontalSpeed = 0.0f;
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
  
  // Guardar velocidad horizontal para animación
  lastHorizontalSpeed = deltaX / delta;
  
  position.x += deltaX;
  
  // Colisión horizontal
  Rectangle playerRect = {position.x, position.y - height, width, height};
  for (auto &envItem : envItems) {
    if (envItem.blocking && CheckCollisionRecs(playerRect, envItem.rect)) {
      if (deltaX > 0) {
        position.x = envItem.rect.x - width;
      } else if (deltaX < 0) {
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
  float nextY = position.y + (speed * delta);
  
  for (auto &envItem : envItems) {
    if (envItem.blocking &&
        envItem.rect.x <= position.x + width &&
        envItem.rect.x + envItem.rect.width >= position.x) {
      
      // Colisión desde arriba (cayendo)
      if (speed > 0 && 
          position.y <= envItem.rect.y &&
          nextY >= envItem.rect.y) {
        hitObstacle = true;
        speed = 0.0f;
        position.y = envItem.rect.y;
        canJump = true;
        break;
      }
      // Colisión desde abajo (saltando)
      else if (speed < 0 && 
               position.y - height >= envItem.rect.y + envItem.rect.height &&
               nextY - height <= envItem.rect.y + envItem.rect.height) {
        hitObstacle = true;
        speed = 0.0f;
        position.y = envItem.rect.y + envItem.rect.height + height;
        break;
      }
    }
  }
  
  if (!hitObstacle) {
    position.y += speed * delta;
    speed += GRAVITY * delta;
    canJump = false;
  }
  
  // Actualizar animación
  updateAnimation(delta);
}

void Player::draw() {
    // Refrescar textura por si se cargó tarde
    if (texture.id == 0) {
        texture = TextureManager::Instance().Get("player");
        if (texture.id != 0) {
            totalFrames = 7;
            frameRec = { 0.0f, 0.0f, 50.0f, 37.0f };
        }
    }
    
    if (texture.id != 0) {
        // ESCALA: Ajusta para cambiar el tamaño visual del sprite
        float scale = 2.0f;
        float renderWidth = 50.0f * scale;
        float renderHeight = 37.0f * scale;

        // Ajuste visual para bajar el sprite
        float spriteFootOffset = 3.0f;   // ← PRUEBA entre 6 y 14

        Rectangle destRec = {
            position.x - renderWidth * 0.5f, 
            position.y - renderHeight + spriteFootOffset,
            renderWidth,
            renderHeight
        };

        
        // Invertir si mira a la izquierda
        Rectangle source = frameRec;
        if (!isFacingRight) {
            source.width = -source.width;
        }
        
        DrawTexturePro(texture, source, destRec, {0,0}, 0.0f, WHITE);
        
        // DEBUG: Descomenta para ver el hitbox de colisión
        // DrawRectangleLinesEx(bounds(), 2, RED);
    } else {
        // Fallback
        Rectangle r = bounds();
        DrawRectangleRec(r, (Color){66, 135, 245, 255});
        DrawRectangleLinesEx(r, 2, BLACK);
    }
}

void Player::updateAnimation(float deltaTime) {
    // Configuración de animaciones
    struct AnimConfig {
        int row;        // Fila en el spritesheet (0-indexed)
        int numFrames;  // Cuántos frames usar de esa fila
        float speed;    // FPS de la animación
    };
    
    // CONFIGURACIÓN PARA TU SPRITESHEET 350x407 (7 cols × 11 filas)
    static const AnimConfig animConfigs[] = {
        {0, 4, 8.0f},   // IDLE: fila 0, 7 frames, 8 FPS
        {1, 6, 12.0f},  // RUNNING: fila 1, 7 frames, 12 FPS
        {2, 7, 10.0f},  // JUMPING: fila 3, 3 frames, 10 FPS
        {3, 3, 10.0f}   // FALLING: fila 4, 3 frames, 10 FPS
    };
    
    // Determinar estado de animación según física
    AnimState newAnim = IDLE;
    
    if (speed < -50.0f) {
        newAnim = JUMPING;
    } else if (speed > 50.0f && !canJump) {
        newAnim = FALLING;
    } else if (std::fabs(lastHorizontalSpeed) > 10.0f) {
        newAnim = RUNNING;
    } else {
        newAnim = IDLE;
    }
    
    // Si cambió el estado, reiniciar animación
    if (newAnim != currentAnim) {
        currentFrame = 0;
        frameTime = 0.0f;
        previousAnim = currentAnim;
        currentAnim = newAnim;
    }
    
    const AnimConfig& config = animConfigs[currentAnim];
    frameSpeed = config.speed;
    
    // Actualizar timer de frames
    frameTime += deltaTime;
    
    // Cambiar frame cuando el timer supere el umbral
    if (frameTime >= 1.0f / frameSpeed) {
        frameTime = 0.0f;
        currentFrame++;
        
        // Loop de animación
        if (currentFrame >= config.numFrames) {
            currentFrame = 0;
        }
        
        // Actualizar frameRec para el frame correcto
        if (texture.id != 0) {
            // Cada frame: 50x37 pixels
            frameRec.x = currentFrame * 50.0f;
            frameRec.y = config.row * 37.0f;
            frameRec.width = 50.0f;
            frameRec.height = 37.0f;
        }
    }
}