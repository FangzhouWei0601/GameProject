#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "TextureData.h"

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    // Texture management
    bool loadTexture(const std::string& name, const std::string& path);
    void unloadTexture(const std::string& name);
    TextureData* getTexture(const std::string& name);

    // Resource management
    void initialize();
    void shutdown();

private:
    ResourceManager() = default;
    ~ResourceManager() { shutdown(); }
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, std::unique_ptr<TextureData>> m_textures;

    // Texture loading utilities
    unsigned char* loadTextureData(const std::string& path, int& width, int& height, int& channels);
    void freeTextureData(unsigned char* data);

    GLenum getGLFormat(int channels);
};