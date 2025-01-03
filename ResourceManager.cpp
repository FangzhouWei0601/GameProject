#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "ResourceManager.h"
#include <iostream>

void ResourceManager::initialize() {
    // Initialize any required systems
    // For now, we just ensure OpenGL is ready
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return;
    }
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& path) {
    // Check if texture already exists
    if (m_textures.find(name) != m_textures.end()) {
        std::cout << "Texture " << name << " already loaded" << std::endl;
        return true;
    }

    // Create new texture data
    auto textureData = std::make_unique<TextureData>();
    textureData->name = name;

    // Load image data
    int width, height, channels;
    unsigned char* data = loadTextureData(path, width, height, channels);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }

    // Create OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Get format based on number of channels
    GLenum format = getGLFormat(channels);

    // Load texture data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Store texture data
    textureData->id = textureID;
    textureData->width = width;
    textureData->height = height;
    textureData->channels = channels;

    // Free image data
    freeTextureData(data);

    // Store in map
    m_textures[name] = std::move(textureData);

    std::cout << "Loaded texture: " << name << " (" << width << "x" << height
        << ", " << channels << " channels)" << std::endl;
    return true;
}

TextureData* ResourceManager::getTexture(const std::string& name) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        return it->second.get();
    }
    return nullptr;
}

void ResourceManager::unloadTexture(const std::string& name) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        glDeleteTextures(1, &it->second->id);
        m_textures.erase(it);
    }
}

void ResourceManager::shutdown() {
    for (auto& texture : m_textures) {
        if (texture.second) {
            glDeleteTextures(1, &texture.second->id);
        }
    }
    m_textures.clear();
}

// Image loading utilities
unsigned char* ResourceManager::loadTextureData(const std::string& path, int& width, int& height, int& channels) {
    // Flip images vertically on load (OpenGL expects different texture coordinates)
    stbi_set_flip_vertically_on_load(true);

    // Load image
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load image: " << path << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
    }

    return data;
}

void ResourceManager::freeTextureData(unsigned char* data) {
    stbi_image_free(data);
}

GLenum ResourceManager::getGLFormat(int channels) {
    switch (channels) {
    case 1: return GL_RED;
    case 3: return GL_RGB;
    case 4: return GL_RGBA;
    default:
        std::cerr << "Unsupported number of channels: " << channels << std::endl;
        return GL_RGB;
    }
}