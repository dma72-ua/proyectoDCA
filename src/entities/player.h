#pragma once
extern "C" {
#include "../vendor/include/raylib.h"
}
#include <vector>
#include "../managers/envItem.h"

class Player {
public:
    // Estado (según tu implementación)
    Vector2 position;     // y = pies
    float   speed = 0.0f; // velocidad vertical (vy)
    bool    canJump = false;

    // AABB para enemigos/cámara
    float width  = 40.0f;
    float height = 48.0f;

    // Constantes que usa tu player.cpp
    static constexpr float PLAYER_MOVE_SPEED = 200.0f; // horizontal
    static constexpr float PLAYER_JUMP_SPEED = 350.0f; // salto

    // Constructores
    Player() : position{0,0}, speed(0.0f), canJump(false) {}
    Player(Vector2 position, float speed, bool canJump);

    // API original en tu player.cpp
    void updatePlayer(float delta, std::vector<EnvItem>& envItems);
    void draw(); // sin const para cuadrar con tu .cpp

    // Aliases usados por main.cpp
    void UpdatePlayer(const std::vector<EnvItem>& envItems, float dt) {
        auto &nc = const_cast<std::vector<EnvItem>&>(envItems);
        updatePlayer(dt, nc);
    }
    void Draw() { draw(); } // sin const

    // Utilidades para enemigos/cámara
    Rectangle bounds() const {
        return { position.x - width*0.5f, position.y - height, width, height };
    }
    float vy() const { return speed; }
    void  setVy(float v) { speed = v; }
    void  bounce(float v = -300.0f) { speed = v; }
    void  reset(Vector2 p) { position = p; speed = 0.0f; canJump = false; }

    // --- Modeling Support ---
    Texture2D texture = {0};
    Rectangle frameRec = {0,0,0,0};
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 8;
    bool isFacingRight = true;
private:
    float frameTime = 0.0f;
    float frameSpeed = 10.0f; // Frames por segundo
    int totalFrames = 1;
    
    enum AnimState {
        IDLE,
        RUNNING,
        JUMPING,
        FALLING
    };
    AnimState currentAnim = IDLE;
    AnimState previousAnim = IDLE;
    
    void updateAnimation(float deltaTime);
    float lastHorizontalSpeed = 0.0f; // Para detectar movimiento horizontal
};
