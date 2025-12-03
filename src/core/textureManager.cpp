#include "textureManager.h"
#include <iostream>
#include <vector>

void TextureManager::Load(const std::string& name, const std::string& fileName) {
    if (textures.find(name) != textures.end()) {
        return; // Ya existe
    }

    // Cargar imagen en CPU primero para procesarla
    Image img = LoadImage(fileName.c_str());

    if (img.data != NULL) {
        // SOLO aplicar flood fill a texturas de personajes/sprites pequeños
        // NO aplicar a texturas de entorno (bricks, pipe, etc.)
        ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        img.mipmaps = 1;

        img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        
        bool shouldProcessTransparency = (name == "player" || name == "coin" || name == "enemy" || name == "star" || name == "heart");

        if (shouldProcessTransparency) {
            // 1. Flood Fill Transparency (BFS)
            // Usamos BFS para eliminar el fondo conectado desde (0,0)
            // Esto evita borrar pixeles del personaje que coincidan con el fondo
            Color bgColor = GetImageColor(img, 0, 0);

            if (bgColor.a > 250) {
                int width = img.width;
                int height = img.height;
                Color* pixels = LoadImageColors(img);

                // Queue para BFS (guardamos índices)
                std::vector<int> queue;
                queue.reserve(width * height / 4);
                queue.push_back(0); // Empezar en (0,0)

                // Array de visitados
                std::vector<bool> visited(width * height, false);
                visited[0] = true;

                int minX = width, minY = height, maxX = 0, maxY = 0;
                bool foundOpaque = false;

                // Tolerancia reducida (valor conservador)
                int toleranceSq = 1000; // ~31 distance

                size_t head = 0;
                while (head < queue.size()) {
                    int idx = queue[head++];
                    pixels[idx] = BLANK; // Hacer transparente

                    int cx = idx % width;
                    int cy = idx / width;

                    // Vecinos (4-connectivity)
                    int neighbors[4] = { idx - 1, idx + 1, idx - width, idx + width };
                    int nx[4] = { cx - 1, cx + 1, cx, cx };
                    int ny[4] = { cy, cy, cy - 1, cy + 1 };

                    for (int k = 0; k < 4; k++) {
                        if (nx[k] >= 0 && nx[k] < width && ny[k] >= 0 && ny[k] < height) {
                            int nIdx = neighbors[k];
                            if (!visited[nIdx]) {
                                Color c = pixels[nIdx];
                                int rDiff = c.r - bgColor.r;
                                int gDiff = c.g - bgColor.g;
                                int bDiff = c.b - bgColor.b;

                                if ((rDiff * rDiff + gDiff * gDiff + bDiff * bDiff) < toleranceSq) {
                                    visited[nIdx] = true;
                                    queue.push_back(nIdx);
                                }
                            }
                        }
                    }
                }

                // Re-scan for autocrop bounds (pixeles NO visitados/transparentes)
                for (int i = 0; i < width * height; i++) {
                    if (pixels[i].a > 0) {
                        int x = i % width;
                        int y = i / width;
                        if (x < minX) minX = x;
                        if (y < minY) minY = y;
                        if (x > maxX) maxX = x;
                        if (y > maxY) maxY = y;
                        foundOpaque = true;
                    }
                }

                UnloadImage(img);

                img.data = pixels;
                img.width = width;
                img.height = height;
                img.mipmaps = 1;
                img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

                // 2. Auto-Crop
                // NO auto-croppear spritesheets grandes (p. ej. 'player', 'enemy', 'star')
                // porque las coordenadas de frame del código dependen del tamaño original.
                bool allowAutoCrop = !(name == "player" || name == "enemy" || name == "star");
                if (allowAutoCrop && foundOpaque && (minX > 0 || minY > 0 || maxX < width - 1 || maxY < height - 1)) {
                    Rectangle cropRect = { (float)minX, (float)minY, (float)(maxX - minX + 1), (float)(maxY - minY + 1) };
                    ImageCrop(&img, cropRect);
                }
            }
        }

        // 3. Resize inteligente para mapa
        // Si es una textura de mapa (bricks/pipe) y es muy grande, la redimensionamos
        // para que encaje mejor en los tiles de 32x32
        if (name == "bricks" || name == "pipe") {
            if (img.width > 64 || img.height > 64) {
                ImageResize(&img, 64, 64);
            }
        }

        // 4. Cargar textura final
        Texture2D tex = LoadTextureFromImage(img);
        UnloadImage(img);
        textures[name] = tex;

        std::cout << "TextureManager: Loaded " << name << " successfully." << std::endl;
    }
    else {
        // Fallback
        if (name == "pipe") {
            // Generar tubería procedural
            Image imgPipe = GenImageColor(32, 32, Color{ 0, 0, 0, 0 }); // Transparente base
            // Dibujar cuerpo verde
            ImageDrawRectangle(&imgPipe, 2, 0, 28, 32, Color{ 0, 200, 0, 255 });
            // Brillo
            ImageDrawRectangle(&imgPipe, 6, 0, 4, 32, Color{ 100, 255, 100, 255 });
            // Borde
            ImageDrawRectangleLines(&imgPipe, Rectangle{ 2, 0, 28, 32 }, 1, Color{ 0, 100, 0, 255 });

            Texture2D tex = LoadTextureFromImage(imgPipe);
            UnloadImage(imgPipe);
            textures[name] = tex;
        }
        else {
            Image imgFallback = GenImageColor(32, 32, MAGENTA);
            Texture2D tex = LoadTextureFromImage(imgFallback);
            UnloadImage(imgFallback);
            textures[name] = tex;
        }
        std::cerr << "TextureManager: Failed to load " << fileName << ", using fallback." << std::endl;
    }
}

Texture2D TextureManager::Get(const std::string& name) {
    if (textures.find(name) != textures.end()) {
        return textures[name];
    }
    // Retornar textura vacía o default si no existe
    return { 0 };
}

void TextureManager::UnloadAll() {
    for (auto& kv : textures) {
        UnloadTexture(kv.second);
    }
    textures.clear();
}