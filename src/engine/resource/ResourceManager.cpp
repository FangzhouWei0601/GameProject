#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <iostream>
#include "../../../include/headers/resource/ResourceManager.h"

namespace {
    bool createDirectoryIfNotExists(const std::string& path) {
        try {
            if (!std::filesystem::exists(path)) {
                return std::filesystem::create_directories(path);
            }
            return true;
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Failed to create directory: " << e.what() << std::endl;
            return false;
        }
    }
}

void ResourceManager::initialize() {
    // Initialize any required systems
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return;
    }

    m_soundEngine = irrklang::createIrrKlangDevice();
    if (!m_soundEngine) {
        std::cerr << "Failed to create sound engine!" << std::endl;
    }
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& path) {
    // Check if texture already exists
    if (m_textures.find(name) != m_textures.end()) {
        logError("Texture already loaded", name, path);
        return true;
    }
    // 确保目录存在
    std::filesystem::path texturePath(path);
    if (!std::filesystem::exists(texturePath.parent_path())) {
        std::filesystem::create_directories(texturePath.parent_path());
        std::cout << "Created directory: " << texturePath.parent_path().string() << std::endl;
    }

    // 检查文件是否存在
    if (!std::filesystem::exists(path)) {
        logError("Texture file does not exist", name, path);
        return false;
    }

    // Create new texture data
    auto textureData = std::make_unique<TextureData>();
    textureData->name = name;

    // Load image data
    int width, height, channels;
    unsigned char* data = loadTextureData(path, width, height, channels);
    if (!data) {
        logError("Failed to load texture data", name, path);
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

    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        // 如果是同一个文件，返回true
        // 如果是不同文件，返回false或重新加载
        std::cout << "Texture " << name << " already loaded. Skipping..." << std::endl;
        return true;
    }

    std::cout << "Loaded texture: " << name << " (" << width << "x" << height
        << ", " << channels << " channels)" << std::endl;
    return true;
}

TextureData* ResourceManager::getTexture(const std::string& name) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        // 只在纹理查找失败时输出信息
        return it->second.get();
    }
    // 只输出错误信息
    std::cout << "Texture not found: " << name << std::endl;
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

    for (auto& [name, source] : m_sounds) {
        if (source) {
            source->drop();
        }
    }
    m_sounds.clear();

    if (m_soundEngine) {
        m_soundEngine->drop();
        m_soundEngine = nullptr;
    }
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

bool ResourceManager::loadMapConfig(const std::string& mapId, nlohmann::json& outJson) {
    std::string path = getMapPath(mapId);
    return loadJsonFile(path, outJson);
}

bool ResourceManager::loadMechanismConfig(const std::string& type,
    const std::string& id,
    nlohmann::json& outJson) {
    std::string path = getMechanismPath(type, id);

    // 如果文件不存在，不视为错误
    if (!std::filesystem::exists(path)) {
        return true;
    }

    return loadJsonFile(path, outJson);
}

bool ResourceManager::loadJsonFile(const std::string& path, nlohmann::json& outJson) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return false;
        }

        file >> outJson;
        return true;
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
}

bool ResourceManager::loadSound(const std::string& name, const std::string& path) {
    if (!m_soundEngine) return false;

    // Check if sound already exists
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        std::cout << "Sound " << name << " already loaded" << std::endl;
        return true;
    }

    auto source = m_soundEngine->addSoundSourceFromFile(path.c_str());
    if (!source) {
        std::cerr << "Failed to load sound: " << path << std::endl;
        return false;
    }

    m_sounds[name] = source;
    return true;
}

void ResourceManager::unloadSound(const std::string& name) {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        if (it->second) {
            it->second->drop();
        }
        m_sounds.erase(it);
    }
}

void ResourceManager::createResourceDirectories() {
    // 创建基础目录结构
    createDirectoryIfNotExists(getBasePath());

    // 创建纹理目录
    createDirectoryIfNotExists(getTexturePath("characters"));
    createDirectoryIfNotExists(getTexturePath("backgrounds"));
    createDirectoryIfNotExists(getTexturePath("ui"));

    // 创建地图目录
    createDirectoryIfNotExists(getBasePath() + "maps/areas");
    createDirectoryIfNotExists(getBasePath() + "maps/mechanisms/triggers");
    createDirectoryIfNotExists(getBasePath() + "maps/mechanisms/sequences");

    // 创建音频目录
    createDirectoryIfNotExists(getAudioPath("bgm"));
    createDirectoryIfNotExists(getAudioPath("sfx"));
}

// ResourceManager.cpp
void ResourceManager::logError(const std::string& message,
    const std::string& resourceName,
    const std::string& resourcePath) {
    m_errors.push_back({ message, resourceName, resourcePath });
    std::cerr << "Resource Error: " << message;
    if (!resourceName.empty()) std::cerr << " [" << resourceName << "]";
    if (!resourcePath.empty()) std::cerr << " Path: " << resourcePath;
    std::cerr << std::endl;
}

bool ResourceManager::preloadResources(const std::string& configPath) {
    PreloadConfig config;
    if (!loadPreloadConfig(configPath, config)) {
        logError("Failed to load preload config", "", configPath);
        return false;
    }

    bool success = true;

    // 预加载纹理
    for (const auto& texturePath : config.textures) {
        std::string name = std::filesystem::path(texturePath).stem().string();
        if (!loadTexture(name, texturePath)) {
            success = false;
        }
    }

    // 预加载音频
    for (const auto& soundPath : config.sounds) {
        std::string name = std::filesystem::path(soundPath).stem().string();
        if (!loadSound(name, soundPath)) {
            success = false;
        }
    }

    return success;
}

bool ResourceManager::loadPreloadConfig(const std::string& configPath, PreloadConfig& config) {
    nlohmann::json json;
    if (!loadJsonFile(configPath, json)) {
        return false;
    }

    try {
        if (json.contains("textures")) {
            for (const auto& texture : json["textures"]) {
                config.textures.push_back(texture.get<std::string>());
            }
        }

        if (json.contains("sounds")) {
            for (const auto& sound : json["sounds"]) {
                config.sounds.push_back(sound.get<std::string>());
            }
        }

        if (json.contains("maps")) {
            for (const auto& map : json["maps"]) {
                config.maps.push_back(map.get<std::string>());
            }
        }

        return true;
    }
    catch (const nlohmann::json::exception& e) {
        logError("Failed to parse preload config: " + std::string(e.what()), "", configPath);
        return false;
    }
}
