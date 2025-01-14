#define STB_IMAGE_IMPLEMENTATION
#include <../../include/stb/stb_image.h>
#include <iostream>
#include "../../../include/headers/resource/ResourceManager.h"
#include "../../../include/headers/audio/AudioManager.h"
#include <windows.h>

// Path management
std::string ResourceManager::getExecutablePath() const {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::string(buffer);
}

void ResourceManager::setWorkingDirectory(const std::string& path) {
    m_workingDirectory = path;
    SetCurrentDirectoryA(path.c_str());
}

std::string ResourceManager::resolvePath(const std::string& relativePath) const {
    if (relativePath.empty()) return m_workingDirectory;

    std::filesystem::path fullPath = std::filesystem::path(m_workingDirectory) / relativePath;
    return fullPath.string();
}

bool ResourceManager::initializeWorkingDirectory() {
    try {
        // Get executable path
        std::filesystem::path exePath = getExecutablePath();
        std::filesystem::path workDir = exePath.parent_path();

        // Set as working directory
        setWorkingDirectory(workDir.string());

        // Ensure resource directory exists
        std::filesystem::path resourceDir = workDir / "resources";
        if (!std::filesystem::exists(resourceDir)) {
            std::filesystem::create_directories(resourceDir);
        }

        std::cout << "Working directory set to: " << workDir.string() << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        DEBUG_LOG_ERROR("Failed to initialize working directory: " << e.what());
        return false;
    }
}

void ResourceManager::initialize() {
    // init OpenGL
    if (!gladLoadGL()) {
        DEBUG_LOG_ERROR("Failed to initialize OpenGL context");
        return;
    }

    if (m_soundEngine) {
        m_soundEngine->drop();
        m_soundEngine = nullptr;
    }

    m_soundEngine = irrklang::createIrrKlangDevice();
    if (!m_soundEngine) {
        DEBUG_LOG_ERROR("Failed to create sound engine");
        return; 
    }

    AudioManager::getInstance().initialize();
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

void ResourceManager::createResourceDirectories() {
    std::vector<std::string> directories = {
        getTexturePath("characters"),
        getTexturePath("backgrounds"),
        getTexturePath("ui"),
        getBasePath() + "maps/areas",
        getBasePath() + "maps/mechanisms/triggers",
        getBasePath() + "maps/mechanisms/sequences",
        getAudioPath("bgm"),
        getAudioPath("sfx")
    };

    for (const auto& dir : directories) {
        try {
            std::filesystem::create_directories(dir);
            std::cout << "Created directory: " << dir << std::endl;
        }
        catch (const std::filesystem::filesystem_error& e) {
            DEBUG_LOG_ERROR("Failed to create directory " << dir << ": " << e.what());
        }
    }
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& path) {
    // Check if texture already exists
    if (m_textures.find(name) != m_textures.end()) {
        std::cout << "Texture " << name << " already loaded" << std::endl;
        return true;
    }

    std::string resolvedPath = resolvePath(path);
    // Check if file exists
    if (!std::filesystem::exists(resolvedPath)) {
        DEBUG_LOG_ERROR("Failed to load texture: " << name << " Path: " << resolvedPath);
        return false;
    }

    // Create new texture data
    auto textureData = std::make_unique<TextureData>();
    textureData->name = name;

    // Load image data
    int width, height, channels;
    unsigned char* data = loadTextureData(resolvedPath, width, height, channels);
    if (!data) {
        DEBUG_LOG_ERROR("Failed to load texture data: " << name << " Path: " << resolvedPath);
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

    // Load texture data to GPU and generate mipmaps
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

void ResourceManager::unloadTexture(const std::string& name) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        glDeleteTextures(1, &it->second->id);
        m_textures.erase(it);
    }
}

TextureData* ResourceManager::getTexture(const std::string& name) {
    auto it = m_textures.find(name);
    if (it == m_textures.end()) {
        DEBUG_LOG_ERROR("Texture not found: " << name);
        return nullptr;
    }
    return it->second.get();
}

unsigned char* ResourceManager::loadTextureData(const std::string& path, int& width, int& height, int& channels) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        DEBUG_LOG_ERROR("Failed to load image: " << path << " STB Error: " << stbi_failure_reason());
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
        DEBUG_LOG_ERROR("Unsupported number of channels: " << channels);
        return GL_RGB;
    }
}

bool ResourceManager::loadSound(const std::string& name, const std::string& path) {
    if (!m_soundEngine) {
        DEBUG_LOG_ERROR("Sound engine not initialized");
        return false;
    }

    std::string resolvedPath = resolvePath(path);
    if (!std::filesystem::exists(resolvedPath)) {
        DEBUG_LOG_WARN("Sound file not found: " << resolvedPath);
        return true; 
    }

    auto source = m_soundEngine->addSoundSourceFromFile(resolvedPath.c_str());
    if (!source) {
        DEBUG_LOG_ERROR("Failed to load sound: " << name);
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

bool ResourceManager::loadMapConfig(const std::string& mapId, nlohmann::json& outJson) {
    std::string path = getMapPath(mapId);
    if (!loadJsonFile(path, outJson)) {
        DEBUG_LOG_ERROR("Failed to load map config for: " << mapId);
        return false;
    }
    return true;
}

bool ResourceManager::loadMechanismConfig(const std::string& type, const std::string& id, nlohmann::json& outJson) {
    std::string path = getMechanismPath(type, id);

    if (!std::filesystem::exists(path)) {
        DEBUG_LOG("No mechanism config found: " << path);
        return true;  // 
    }

    return loadJsonFile(path, outJson);
}

bool ResourceManager::loadJsonFile(const std::string& path, nlohmann::json& outJson) {
    try {
        std::string resolvedPath = resolvePath(path);
        std::ifstream file(resolvedPath);
        if (!file.is_open()) {
            DEBUG_LOG_ERROR("Failed to open file: " << resolvedPath);
            return false;
        }

        file >> outJson;
        return true;
    }
    catch (const nlohmann::json::exception& e) {
        DEBUG_LOG_ERROR("JSON parsing error for " << path << ": " << e.what());
        return false;
    }
}

// Preload resources
bool ResourceManager::preloadResources(const std::string& configPath) {
    PreloadConfig config;
    if (!loadPreloadConfig(configPath, config)) {
        DEBUG_LOG_ERROR("Failed to load preload config", "", configPath);
        return false;
    }

    bool success = true;

    // Preload textures
    for (const auto& texturePath : config.textures) {
        std::string name = std::filesystem::path(texturePath).stem().string();
        if (!loadTexture(name, texturePath)) {
            success = false;
        }
    }

    // Preload audio
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
    std::string resolvedPath = resolvePath(configPath);

    if (!loadJsonFile(resolvedPath, json)) {
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
        DEBUG_LOG_ERROR("Failed to parse preload config: " << configPath << " Error: " << e.what());
        return false;
    }
}