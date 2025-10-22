#include "enemy.h"

static constexpr float GRAVITY_E   = 900.0f;  // px/s^2
static constexpr float ENEMY_SPEED = 80.0f;   // px/s

Enemy::Enemy(Vector2 pos, float w, float h, int dirSign) {
    rect = { pos.x, pos.y, w, h };
    vel  = { dirSign >= 0 ? ENEMY_SPEED : -ENEMY_SPEED, 0.0f };
    dir  = (dirSign >= 0) ? +1 : -1;
    alive = true;
    spawnPos = pos;
}

void Enemy::kill() { alive = false; }

void Enemy::reset(Vector2 pos, int dirSign) {
    rect.x = pos.x; rect.y = pos.y;
    vel  = { dirSign >= 0 ? ENEMY_SPEED : -ENEMY_SPEED, 0.0f };
    dir  = (dirSign >= 0) ? +1 : -1;
    alive = true;
    spawnPos = pos;
}

static inline bool Intersects(const Rectangle& a, const Rectangle& b) {
    return CheckCollisionRecs(a, b);
}

void Enemy::update(float dt, const std::vector<EnvItem>& envItems) {
    if (!alive) return;

    // Gravedad y eje Y
    vel.y += GRAVITY_E * dt;
    rect.y += vel.y * dt;
    for (const auto& e : envItems) {
        if (!e.blocking) continue;
        if (Intersects(rect, e.rect)) {
            if (vel.y > 0) { rect.y = e.rect.y - rect.height; vel.y = 0; }
            else if (vel.y < 0) { rect.y = e.rect.y + e.rect.height; vel.y = 0; }
        }
    }

    // Movimiento horizontal y rebote
    vel.x = dir * ENEMY_SPEED;
    rect.x += vel.x * dt;
    for (const auto& e : envItems) {
        if (!e.blocking) continue;
        if (Intersects(rect, e.rect)) {
            if (vel.x > 0) rect.x = e.rect.x - rect.width;
            else           rect.x = e.rect.x + e.rect.width;
            dir *= -1;
            vel.x = dir * ENEMY_SPEED;
        }
    }
}

void Enemy::draw() const {
    if (!alive) return;
    // cuerpo
    DrawRectangleRec(rect, (Color){168,94,45,255});
    // sombreado
    DrawRectangle(rect.x, rect.y + rect.height - 6, rect.width, 6, (Color){110,58,28,255});
    // ojos
    float eyeW = rect.width*0.18f, eyeH = rect.height*0.28f;
    Rectangle eyeL = { rect.x + rect.width*0.28f - eyeW*0.5f, rect.y + rect.height*0.25f, eyeW, eyeH };
    Rectangle eyeR = { rect.x + rect.width*0.72f - eyeW*0.5f, rect.y + rect.height*0.25f, eyeW, eyeH };
    DrawRectangleRec(eyeL, RAYWHITE); DrawRectangleRec(eyeR, RAYWHITE);
    DrawRectangle(eyeL.x + eyeW*0.35f, eyeL.y + eyeH*0.45f, eyeW*0.30f, eyeH*0.40f, BLACK);
    DrawRectangle(eyeR.x + eyeW*0.35f, eyeR.y + eyeH*0.45f, eyeW*0.30f, eyeH*0.40f, BLACK);
}
