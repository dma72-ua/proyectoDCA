#pragma once
extern "C" {
#include "../vendor/include/raylib.h"
}

class Star {
public:
    Rectangle rect;
    bool collected = false;

    Star(Vector2 pos, float size = 24.0f);
    void startCollect();
    bool checkCollision(Rectangle other);
    void draw() const;
    Rectangle bounds() const;

private:
    float collectTime = 0.0f;
    Texture2D texture;
};
