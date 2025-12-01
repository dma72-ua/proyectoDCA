#pragma once
#include <map>
#include <string>
#include <raylib.h>

class TextureManager {
public:
    static TextureManager& Instance() {
        static TextureManager instance;
        return instance;
    }

    // Borrar copia y asignación
    TextureManager(const TextureManager&) = delete;
    void operator=(const TextureManager&) = delete;

    void Load(const std::string& name, const std::string& fileName);
    Texture2D Get(const std::string& name);
    void UnloadAll();

private:
    TextureManager() {}
    ~TextureManager() { UnloadAll(); }

    std::map<std::string, Texture2D> textures;
};
