#pragma once
extern "C" {
#include "../vendor/include/raylib.h"
}
#include <vector>
#include "../managers/envItem.h"

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

    // --- Modeling Support ---
    Texture2D texture = {0};
    Rectangle frameRec = {0,0,0,0};
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 8;

private:
    Vector2 spawnPos;
};
