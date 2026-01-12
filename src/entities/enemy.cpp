#include "enemy.h"
#include "../core/textureManager.h"

static constexpr float GRAVITY_E = 900.0f;  // px/s^2
static constexpr float ENEMY_SPEED = 80.0f; // px/s

Enemy::Enemy(Vector2 pos, float w, float h, int dirSign) {
  rect = {pos.x, pos.y, w, h};
  vel = {dirSign >= 0 ? ENEMY_SPEED : -ENEMY_SPEED, 0.0f};
  dir = (dirSign >= 0) ? +1 : -1;
  alive = true;
  spawnPos = pos;

  startingPos = pos;
  startingDir = dirSign;

  // Intentar cargar textura
  this->texture = TextureManager::Instance().Get("enemy");
  if (this->texture.id != 0) {
    bool isStrip = (this->texture.width > this->texture.height * 2);
    int numFrames = isStrip ? 4 : 1;
    float frameWidth = (float)this->texture.width / (float)numFrames;
    this->frameRec = {0.0f, 0.0f, frameWidth, (float)this->texture.height};
  }
}

void Enemy::kill() { alive = false; }

void Enemy::reset(Vector2 pos, int dirSign) {
  rect.x = pos.x;
  rect.y = pos.y;
  vel = {dirSign >= 0 ? ENEMY_SPEED : -ENEMY_SPEED, 0.0f};
  dir = (dirSign >= 0) ? +1 : -1;
  alive = true;
  spawnPos = pos;
}

static inline bool Intersects(const Rectangle &a, const Rectangle &b) {
  return CheckCollisionRecs(a, b);
}

void Enemy::update(float dt, const std::vector<EnvItem> &envItems) {
  if (!alive)
    return;

  // Gravedad y eje Y
  vel.y += GRAVITY_E * dt;
  rect.y += vel.y * dt;
  for (const auto &e : envItems) {
    if (!e.blocking)
      continue;
    if (Intersects(rect, e.rect)) {
      if (vel.y > 0) {
        rect.y = e.rect.y - rect.height;
        vel.y = 0;
      } else if (vel.y < 0) {
        rect.y = e.rect.y + e.rect.height;
        vel.y = 0;
      }
    }
  }

  // Movimiento horizontal y rebote
  vel.x = dir * ENEMY_SPEED;
  rect.x += vel.x * dt;
  for (const auto &e : envItems) {
    if (!e.blocking)
      continue;
    if (Intersects(rect, e.rect)) {
      if (vel.x > 0)
        rect.x = e.rect.x - rect.width;
      else
        rect.x = e.rect.x + e.rect.width;
      dir *= -1;
      vel.x = dir * ENEMY_SPEED;
    }
  }

  // Animation Update
  if (texture.id != 0) {
    if (frameRec.width < texture.width) {
      framesCounter++;
      if (framesCounter >= (60 / framesSpeed)) {
        framesCounter = 0;
        currentFrame++;
        if (currentFrame > 3)
          currentFrame = 0;
        frameRec.x = (float)currentFrame * frameRec.width;
      }
    }
  }
}

void Enemy::draw() const {
  if (!alive)
    return;

  // Hack para usar TextureManager en método const (o mutable en header)
  // O simplemente asumimos que ya se cargó en constructor.
  // Si queremos reintentar carga, necesitamos quitar const o usar const_cast,
  // pero por simplicidad usaremos la textura si está cargada.

  if (texture.id != 0) {
    Rectangle destRec = {rect.x, rect.y, rect.width, rect.height};
    Rectangle source = frameRec;
    // Si va a la izquierda, invertimos (dependiendo de cómo sea el sprite
    // original) Asumamos sprite mira derecha
    if (dir == -1)
      source.width = -source.width;

    DrawTexturePro(texture, source, destRec, {0, 0}, 0.0f, WHITE);
  } else {
    // Fallback
    // cuerpo
    Color c = {168, 94, 45, 255};
    DrawRectangleRec(rect, c);
    // sombreado
    DrawRectangle(rect.x, rect.y + rect.height - 6, rect.width, 6,
                  Color{110, 58, 28, 255});
    // ojos
    float eyeW = rect.width * 0.18f, eyeH = rect.height * 0.28f;
    Rectangle eyeL = {rect.x + rect.width * 0.28f - eyeW * 0.5f,
                      rect.y + rect.height * 0.25f, eyeW, eyeH};
    Rectangle eyeR = {rect.x + rect.width * 0.72f - eyeW * 0.5f,
                      rect.y + rect.height * 0.25f, eyeW, eyeH};
    DrawRectangleRec(eyeL, RAYWHITE);
    DrawRectangleRec(eyeR, RAYWHITE);
    DrawRectangle(eyeL.x + eyeW * 0.35f, eyeL.y + eyeH * 0.45f, eyeW * 0.30f,
                  eyeH * 0.40f, BLACK);
    DrawRectangle(eyeR.x + eyeW * 0.35f, eyeR.y + eyeH * 0.45f, eyeW * 0.30f,
                  eyeH * 0.40f, BLACK);
  }
}
