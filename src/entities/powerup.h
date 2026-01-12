#pragma once
extern "C" {
#include "../vendor/include/raylib.h"
}

enum class PowerUpType {
    SPEED_BOOTS,     // Botas de velocidad - aumenta velocidad de movimiento
    ANGEL_WINGS,     // Alas de ángel - permite planear
    SHIELD           // Escudo - absorbe un golpe
};

class PowerUp {
public:
    Rectangle rect;
    PowerUpType type;
    bool collected = false;
    bool active = false;
    
    // Para power-ups temporales
    float duration = 0.0f;        // Duración del efecto (0 = permanente hasta perder vida)
    float activationTime = 0.0f;  // Cuando se activó
    
    PowerUp(Vector2 pos, PowerUpType type, float size = 32.0f, float duration = 0.0f);
    
    void startCollect();
    bool checkCollision(Rectangle other);
    void draw() const;
    Rectangle bounds() const;
    bool isExpired() const;
    
    // Getters para efectos visuales
    Color getColor() const;
    const char* getName() const;
    const char* getIcon() const;

private:
    float collectTime = 0.0f;
    Texture2D texture;
    
    // Animación de flotación
    float floatOffset = 0.0f;
    float floatSpeed = 2.0f;
};
