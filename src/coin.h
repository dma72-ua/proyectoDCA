#pragma once
extern "C" {
#include "../vendor/include/raylib.h"
}

class Coin {
public:
    Rectangle rect;
    bool collected = false;

    Coin(Vector2 pos, float size = 20.0f);
    void startCollect();
    bool checkCollision(Rectangle other);
    void draw() const;
    Rectangle bounds() const;

private:
    float collectTime = 0.0f;
};