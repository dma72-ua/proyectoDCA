#include "coin.h"
#include "core/textureManager.h"

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
    Texture2D tex = TextureManager::Instance().Get("coin");
    if (tex.id == 0) return; 

    if (!collected) {
        
        DrawTexturePro(
            tex,
            {0, 0, (float)tex.width, (float)tex.height}, // source
            rect,                                         // destino
            {rect.width * 0.5f, rect.height * 0.5f},      // origen (centro)
            0.0f,                                         // ¡rotación = 0!
            WHITE
        );
    } else {
        // Animación de desaparición: escala y transparencia
        float elapsed = GetTime() - collectTime;
        if (elapsed < 0.2f) {
            float scale = 1.0f - elapsed * 5.0f;
            float alpha = 1.0f - (elapsed / 0.2f);
            Color tint = {255, 255, 255, (unsigned char)(255 * alpha)};

            // Ajustar posición para que se encoja desde el centro
            Rectangle animRect = {
                rect.x + rect.width * (1 - scale) * 0.5f,
                rect.y + rect.height * (1 - scale) * 0.5f,
                rect.width * scale,
                rect.height * scale
            };

            DrawTexturePro(
                tex,
                {0, 0, (float)tex.width, (float)tex.height},
                animRect,
                {animRect.width * 0.5f, animRect.height * 0.5f},
                0.0f,
                tint
            );
        }
    }
}

Rectangle Coin::bounds() const {
    return rect;
}