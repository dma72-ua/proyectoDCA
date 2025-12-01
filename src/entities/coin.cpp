#include "coin.h"
#include <cmath>

Coin::Coin(Vector2 pos, float size)
    : rect{pos.x, pos.y, size, size} {}

void Coin::startCollect() {
    collected = true;
    collectTime = GetTime();
}

bool Coin::checkCollision(Rectangle other) {
    if (collected) return false;
    return CheckCollisionRecs(rect, other);
}

void Coin::draw() const {
    Vector2 center = {rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f};
    float baseRadius = rect.width * 0.5f;

    if (!collected) {
        float t = GetTime();
        float pulse = 1.0f + 0.1f * sinf(t * 8.0f);
        float radius = baseRadius * pulse;
        Color gold = {255, 215, 0, (unsigned char)(200 + 55 * sinf(t * 6))};

        DrawCircle(center.x, center.y, radius, gold);
        DrawCircleLines(center.x, center.y, radius, (Color){184, 134, 11, 255});
        DrawCircle(center.x - radius * 0.3f, center.y - radius * 0.3f,
                   radius * 0.25f, (Color){255, 255, 220, 200});
    } else {
        float elapsed = GetTime() - collectTime;
        if (elapsed < 0.2f) {
            float scale = 1.0f - elapsed * 5.0f; 
            unsigned char alpha = (unsigned char)(255 * (1.0f - elapsed / 0.2f));
            DrawCircle(center.x, center.y, baseRadius * scale, (Color){255, 215, 0, alpha});
        }
    }
}

Rectangle Coin::bounds() const {
    return rect;
}