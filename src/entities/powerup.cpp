#include "powerup.h"
#include "../core/textureManager.h"
#include <cmath>

PowerUp::PowerUp(Vector2 pos, PowerUpType type, float size, float duration)
    : rect{pos.x, pos.y, size, size}, type(type), duration(duration) {
    
    // Intentar cargar textura según el tipo
    switch(type) {
        case PowerUpType::SPEED_BOOTS:
            this->texture = TextureManager::Instance().Get("boots");
            floatSpeed = 2.5f;
            break;
        case PowerUpType::ANGEL_WINGS:
            this->texture = TextureManager::Instance().Get("wings");
            floatSpeed = 3.0f;
            break;
        case PowerUpType::SHIELD:
            this->texture = TextureManager::Instance().Get("shield");
            floatSpeed = 2.0f;
            break;
    }
}

void PowerUp::startCollect() {
    collected = true;
    active = true;
    collectTime = GetTime();
    activationTime = GetTime();
}

bool PowerUp::checkCollision(Rectangle other) {
    if (collected) return false;
    return CheckCollisionRecs(rect, other);
}

bool PowerUp::isExpired() const {
    if (!active || duration <= 0.0f) return false;
    return (GetTime() - activationTime) >= duration;
}

Color PowerUp::getColor() const {
    switch(type) {
        case PowerUpType::SPEED_BOOTS:
            return Color{76, 187, 23, 255}; 
        case PowerUpType::ANGEL_WINGS:
            return Color{135, 206, 250, 255}; 
        case PowerUpType::SHIELD:
            return Color{255, 215, 0, 255}; 
        default:
            return MAGENTA;
    }
}

const char* PowerUp::getName() const {
    switch(type) {
        case PowerUpType::SPEED_BOOTS:
            return "Botas de Velocidad";
        case PowerUpType::ANGEL_WINGS:
            return "Alas de Angel";
        case PowerUpType::SHIELD:
            return "Escudo";
        default:
            return "Power-Up";
    }
}

const char* PowerUp::getIcon() const {
    switch(type) {
        case PowerUpType::SPEED_BOOTS:
            return "B";  // Boots
        case PowerUpType::ANGEL_WINGS:
            return "W";  // Wings
        case PowerUpType::SHIELD:
            return "S";  // Shield
        default:
            return "?";
    }
}

void PowerUp::draw() const {
    // Calcular offset de flotación
    float time = GetTime();
    float currentFloatOffset = sinf(time * floatSpeed) * 5.0f;
    
    Vector2 center = {
        rect.x + rect.width * 0.5f, 
        rect.y + rect.height * 0.5f + currentFloatOffset
    };
    
    if (!collected) {
        // Efecto de brillo pulsante
        float pulse = 1.0f + 0.1f * sinf(time * 4.0f);
        float size = rect.width * pulse;
        
        // Dibujar resplandor
        float glowSize = size * 1.3f;
        Color glowColor = getColor();
        glowColor.a = 80;
        DrawCircle(center.x, center.y, glowSize * 0.5f, glowColor);
        
        // Usar textura si está disponible
        if (texture.id != 0) {
            Rectangle destRec = { 
                center.x - size * 0.5f, 
                center.y - size * 0.5f, 
                size, 
                size 
            };
            
            unsigned char alpha = (unsigned char)(220 + 35 * sinf(time * 3));
            DrawTexturePro(texture, 
                {0, 0, (float)texture.width, (float)texture.height},
                destRec, 
                {0, 0}, 
                sinf(time * 2.0f) * 5.0f,  // Rotación suave
                Color{255, 255, 255, alpha});
        } else {
            // Dibujo procedural como fallback
            Color mainColor = getColor();
            
            switch(type) {
                case PowerUpType::SPEED_BOOTS: {
                    // Dibujar una bota estilizada
                    Rectangle boot = {center.x - size * 0.3f, center.y - size * 0.2f, size * 0.6f, size * 0.7f};
                    DrawRectangleRec(boot, mainColor);
                    DrawRectangle(boot.x, boot.y + boot.height - 10, boot.width, 10, Color{50, 150, 20, 255});
                    // Rayo de velocidad
                    DrawTriangle(
                        {boot.x + boot.width, boot.y + boot.height * 0.3f},
                        {boot.x + boot.width + 8, boot.y + boot.height * 0.5f},
                        {boot.x + boot.width, boot.y + boot.height * 0.7f},
                        YELLOW
                    );
                    break;
                }
                
                case PowerUpType::ANGEL_WINGS: {
                    // Dibujar alas
                    float wingW = size * 0.4f;
                    float wingH = size * 0.6f;
                    
                    // Ala izquierda
                    DrawEllipse(center.x - wingW * 0.5f, center.y, wingW, wingH, mainColor);
                    DrawEllipse(center.x - wingW * 0.5f, center.y, wingW * 0.7f, wingH * 0.7f, WHITE);
                    
                    // Ala derecha
                    DrawEllipse(center.x + wingW * 0.5f, center.y, wingW, wingH, mainColor);
                    DrawEllipse(center.x + wingW * 0.5f, center.y, wingW * 0.7f, wingH * 0.7f, WHITE);
                    
                    // Círculo central
                    DrawCircle(center.x, center.y, size * 0.15f, GOLD);
                    break;
                }
                
                case PowerUpType::SHIELD: {
                    // Dibujar escudo
                    float shieldSize = size * 0.8f;
                    
                    // Forma de escudo (hexágono)
                    Vector2 points[6];
                    for (int i = 0; i < 6; i++) {
                        float angle = (i * PI / 3.0f) - PI / 2.0f;
                        points[i] = {
                            center.x + cosf(angle) * shieldSize * 0.5f,
                            center.y + sinf(angle) * shieldSize * 0.5f
                        };
                    }
                    
                    // Dibujar escudo
                    for (int i = 0; i < 6; i++) {
                        DrawTriangle(center, points[i], points[(i + 1) % 6], mainColor);
                    }
                    
                    // Borde
                    for (int i = 0; i < 6; i++) {
                        DrawLineEx(points[i], points[(i + 1) % 6], 2.0f, Color{200, 150, 0, 255});
                    }
                    
                    // Cruz central
                    DrawLineEx(
                        {center.x - shieldSize * 0.2f, center.y},
                        {center.x + shieldSize * 0.2f, center.y},
                        3.0f, WHITE
                    );
                    DrawLineEx(
                        {center.x, center.y - shieldSize * 0.2f},
                        {center.x, center.y + shieldSize * 0.2f},
                        3.0f, WHITE
                    );
                    break;
                }
            }
            
            // Borde exterior
            DrawCircleLines(center.x, center.y, size * 0.5f, Color{255, 255, 255, 200});
        }
        
    } else {
        // Animación de recolección
        float elapsed = GetTime() - collectTime;
        if (elapsed < 0.4f) {
            float scale = 1.0f + elapsed * 3.0f;  // Crece
            float moveUp = elapsed * 100.0f;       // Sube
            unsigned char alpha = (unsigned char)(255 * (1.0f - elapsed / 0.4f));
            
            float size = rect.width * scale;
            Vector2 animCenter = {center.x, center.y - moveUp};
            
            if (texture.id != 0) {
                Rectangle destRec = { 
                    animCenter.x - size * 0.5f, 
                    animCenter.y - size * 0.5f, 
                    size, 
                    size 
                };
                DrawTexturePro(texture, 
                    {0, 0, (float)texture.width, (float)texture.height},
                    destRec, 
                    {0, 0}, 
                    elapsed * 180.0f,  // Rotación rápida
                    Color{255, 255, 255, alpha});
            } else {
                Color col = getColor();
                col.a = alpha;
                DrawCircle(animCenter.x, animCenter.y, size * 0.5f, col);
            }
            
            // Texto "+" que aparece
            if (elapsed > 0.1f) {
                const char* text = "+";
                int textWidth = MeasureText(text, 30);
                DrawText(text, animCenter.x - textWidth/2, animCenter.y - 40, 30, Color{255, 255, 255, alpha});
            }
        }
    }
}

Rectangle PowerUp::bounds() const {
    return rect;
}