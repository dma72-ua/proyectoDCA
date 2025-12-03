#include "star.h"
#include "../core/textureManager.h"
#include <cmath>

Star::Star(Vector2 pos, float size)
    : rect{pos.x, pos.y, size, size} {
    // Try to load texture
    this->texture = TextureManager::Instance().Get("star");
}

void Star::startCollect() {
    collected = true;
    collectTime = GetTime();
}

bool Star::checkCollision(Rectangle other) {
    if (collected) return false;
    return CheckCollisionRecs(rect, other);
}

void Star::draw() const {
    Vector2 center = {rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f};
    float baseRadius = rect.width * 0.5f;

    if (!collected) {
        // Try to use texture if available
        if (texture.id != 0) {
            float t = GetTime();
            float pulse = 1.0f + 0.15f * sinf(t * 5.0f);
            float size = rect.width * pulse;
            
            Rectangle destRec = { 
                center.x - size * 0.5f, 
                center.y - size * 0.5f, 
                size, 
                size 
            };
            
            unsigned char alpha = (unsigned char)(220 + 35 * sinf(t * 4));
            DrawTexturePro(texture, 
                {0, 0, (float)texture.width, (float)texture.height},
                destRec, 
                {0, 0}, 
                0.0f, 
                Color{255, 255, 255, alpha});
        } else {
            // Fallback to procedural drawing
            float t = GetTime();
            float pulse = 1.0f + 0.15f * sinf(t * 5.0f);
            float radius = baseRadius * pulse;
            
            // Draw 5-pointed star
            const int numPoints = 5;
            Vector2 outerPoints[5];
            Vector2 innerPoints[5];
            
            float outerRadius = radius;
            float innerRadius = radius * 0.4f;
            
            // Calculate star points
            for (int i = 0; i < numPoints; i++) {
                float outerAngle = (i * 2.0f * PI / numPoints) - PI / 2.0f;
                float innerAngle = outerAngle + PI / numPoints;
                
                outerPoints[i] = {
                    center.x + outerRadius * cosf(outerAngle),
                    center.y + outerRadius * sinf(outerAngle)
                };
                
                innerPoints[i] = {
                    center.x + innerRadius * cosf(innerAngle),
                    center.y + innerRadius * sinf(innerAngle)
                };
            }
            
            // Draw star filled with triangles
            Color starYellow = {255, 215, 0, (unsigned char)(220 + 35 * sinf(t * 4))};
            for (int i = 0; i < numPoints; i++) {
                int next = (i + 1) % numPoints;
                DrawTriangle(center, outerPoints[i], innerPoints[i], starYellow);
                DrawTriangle(center, innerPoints[i], outerPoints[next], starYellow);
            }
            
            // Draw star outline
            for (int i = 0; i < numPoints; i++) {
                int next = (i + 1) % numPoints;
                DrawLineEx(outerPoints[i], innerPoints[i], 2.0f, BLACK);
                DrawLineEx(innerPoints[i], outerPoints[next], 2.0f, BLACK);
            }
            
            // Add highlight
            float highlightSize = radius * 0.2f;
            DrawCircle(center.x - radius * 0.2f, center.y - radius * 0.2f, 
                       highlightSize, Color{255, 255, 220, 150});
        }
                   
    } else {
        // Collection animation
        float elapsed = GetTime() - collectTime;
        if (elapsed < 0.3f) {
            float scale = 1.0f + elapsed * 2.0f; // Grow
            unsigned char alpha = (unsigned char)(255 * (1.0f - elapsed / 0.3f));
            
            if (texture.id != 0) {
                // Texture-based collection animation
                float size = rect.width * scale;
                Rectangle destRec = { 
                    center.x - size * 0.5f, 
                    center.y - size * 0.5f, 
                    size, 
                    size 
                };
                DrawTexturePro(texture, 
                    {0, 0, (float)texture.width, (float)texture.height},
                    destRec, 
                    {0, 0}, 
                    0.0f, 
                    Color{255, 255, 255, alpha});
            } else {
                // Procedural collection animation
                float radius = baseRadius * scale;
                const int numPoints = 5;
                
                for (int i = 0; i < numPoints; i++) {
                    float outerAngle = (i * 2.0f * PI / numPoints) - PI / 2.0f;
                    float innerAngle = outerAngle + PI / numPoints;
                    
                    Vector2 outerPoint = {
                        center.x + radius * cosf(outerAngle),
                        center.y + radius * sinf(outerAngle)
                    };
                    Vector2 innerPoint = {
                        center.x + radius * 0.4f * cosf(innerAngle),
                        center.y + radius * 0.4f * sinf(innerAngle)
                    };
                    
                    int next = (i + 1) % numPoints;
                    float nextOuterAngle = (next * 2.0f * PI / numPoints) - PI / 2.0f;
                    Vector2 nextOuterPoint = {
                        center.x + radius * cosf(nextOuterAngle),
                        center.y + radius * sinf(nextOuterAngle)
                    };
                    
                    DrawTriangle(center, outerPoint, innerPoint, Color{255, 215, 0, alpha});
                    DrawTriangle(center, innerPoint, nextOuterPoint, Color{255, 215, 0, alpha});
                }
            }
        }
    }
}

Rectangle Star::bounds() const {
    return rect;
}
