#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <iostream>
#include "../../../include/headers/resource/ResourceManager.h"
#include "../../../include/headers/audio/AudioManager.h"
#include "../../../include/headers/CommonDefines.h"

namespace {
    bool createDirectoryIfNotExists(const std::string& path) {
        try {
            if (!std::filesystem::exists(path)) {
                return std::filesystem::create_directories(path);
            }
            return true;
        }
        catch (const std::filesystem::filesystem_error& e) {
            DEBUG_LOG_ERROR("Failed to create directory : " << e.what());
            return false;
        }
    }
}

void ResourceManager::initialize() {
    // Initialize any required systems
    if (!gladLoadGL()) {
        DEBUG_LOG_ERROR("Failed to initialize OpenGL context");
        return;
    }

    m_soundEngine = irrklang::createIrrKlangDevice();
    if (!m_soundEngine) {
        DEBUG_LOG_ERROR("Failed to create sound engine!");
    }

    AudioManager::getInstance().initialize();
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& path) {
    if (m_textures.find(name) != m_textures.end()) {
        DEBUG_LOG_ERROR("Texture already loaded", name, path);
        return true;
    }
    std::filesystem::path texturePath(path);
    if (!std::filesystem::exists(texturePath.parent_path())) {
        std::filesystem::create_directories(texturePath.parent_path());
        DEBUG_LOG("Created directory: " << texturePath.parent_path().string());
    }

    if (!std::filesystem::exists(path)) {
        DEBUG_LOG_ERROR("Texture file does not exist", name, path);
        return false;
    }

    // Create new texture data
    auto textureData = std::make_unique<TextureData>();
    textureData->name = name;

    // Load image data
    int width, height, channels;
    unsigned char* data = loadTextureData(path, width, height, channels);
    if (!data) {
        DEBUG_LOG_ERROR("Failed to load texture data", name, path);
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
        DEBUG_LOG("Texture " << name << " already loaded. Skipping...");
        return true;
    }

    DEBUG_LOG("Loaded texture: " << name << " (" << width << "x" << height<< ", " << channels << " channels)");
    return true;
}

TextureData* ResourceManager::getTexture(const std::string& name) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        return it->second.get();
    }

    DEBUG_LOG_ERROR("Texture not found: " << name);
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

    AudioManager::getInstance().shutdown();
}

// Image loading utilities
unsigned char* ResourceManager::loadTextureData(const std::string& path, int& width, int& height, int& channels) {
    // Flip images vertically on load
    stbi_set_flip_vertically_on_load(true);

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
        DEBUG_LOG("Unsupported number of channels:", channels);
        return GL_RGB;
    }
}

bool ResourceManager::loadMapConfig(const std::string& mapId, nlohmann::json& outJson) {
    std::string path = getMapPath(mapId);
    return loadJsonFile(path, outJson);
}

bool ResourceManager::loadMechanismConfig(const std::string& type, const std::string& id, nlohmann::json& outJson) {
    std::string path = getMechanismPath(type, id);

    if (!std::filesystem::exists(path)) {
        return true;
    }

    return loadJsonFile(path, outJson);
}

bool ResourceManager::loadJsonFile(const std::string& path, nlohmann::json& outJson) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            DEBUG_LOG_ERROR("Failed to open file: " << path);
            return false;
        }

        file >> outJson;
        return true;
    }
    catch (const nlohmann::json::exception& e) {
        DEBUG_LOG_ERROR("JSON parsing error: " << e.what());
        return false;
    }
}

bool ResourceManager::loadSound(const std::string& name, const std::string& path) {
    if (!m_soundEngine) return false;

    // Check if sound already exists
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        DEBUG_LOG_ERROR("Sound " << name << " already loaded");
        return true;
    }

    auto source = m_soundEngine->addSoundSourceFromFile(path.c_str());
    if (!source) {
        DEBUG_LOG_ERROR("Failed to load sound: " << path);
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
    createDirectoryIfNotExists(getBasePath());

    createDirectoryIfNotExists(getTexturePath("characters"));
    createDirectoryIfNotExists(getTexturePath("backgrounds"));
    createDirectoryIfNotExists(getTexturePath("ui"));

    createDirectoryIfNotExists(getBasePath() + "maps/areas");
    createDirectoryIfNotExists(getBasePath() + "maps/mechanisms/triggers");
    createDirectoryIfNotExists(getBasePath() + "maps/mechanisms/sequences");

    createDirectoryIfNotExists(getAudioPath("bgm"));
    createDirectoryIfNotExists(getAudioPath("sfx"));
}

bool ResourceManager::preloadResources(const std::string& configPath) {
    PreloadConfig config;
    if (!loadPreloadConfig(configPath, config)) {
        DEBUG_LOG_ERROR("Failed to load preload config", "", configPath);
        return false;
    }

    bool success = true;

    // Preload Texture
    for (const auto& texturePath : config.textures) {
        std::string name = std::filesystem::path(texturePath).stem().string();
        if (!loadTexture(name, texturePath)) {
            success = false;
        }
    }

    // Preload audio
    const std::vector<std::pair<std::string, std::string>> soundsToLoad = {
        {"button", "resources/audio/sfx/button.wav"},
        {"trigger", "resources/audio/sfx/trigger.wav"},
        {"door", "resources/audio/sfx/door.wav"},
        {"portal", "resources/audio/sfx/portal.wav"}
    };

    for (const auto& [name, path] : soundsToLoad) {
        if (std::filesystem::exists(path)) {
            if (!loadSound(name, path)) {
                success = false;
            }
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
        DEBUG_LOG_ERROR("Failed to parse preload config: " + std::string(e.what()), "", configPath);
        return false;
    }
}


