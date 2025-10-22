#pragma once
#include "raylib.h"
#include <vector>
#include "envItem.h"

class Enemy {
public:
    Rectangle rect;   // x,y,w,h en mundo
    Vector2   vel;    // velocidad
    bool      alive;  // vivo?
    int       dir;    // -1 izq, +1 dcha

    Enemy(Vector2 pos, float w = 32.0f, float h = 32.0f, int dirSign = -1);

    void update(float dt, const std::vector<EnvItem>& envItems);
    void draw() const;
    void kill();
    void reset(Vector2 pos, int dirSign = -1);

    Rectangle bounds() const { return rect; }

private:
    Vector2 spawnPos;
};
