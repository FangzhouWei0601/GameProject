#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "TextureData.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <irrklang/irrKlang.h>
#include <filesystem>
#include <vector>
#include <iostream>
#include "../headers/CommonDefines.h"

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    // Path management methods
    void setWorkingDirectory(const std::string& path);
    std::string getWorkingDirectory() const { return m_workingDirectory; }
    std::string resolvePath(const std::string& relativePath) const;
    bool initializeWorkingDirectory();

    // Texture management
    bool loadTexture(const std::string& name, const std::string& path);
    void unloadTexture(const std::string& name);
    TextureData* getTexture(const std::string& name);

    bool preloadResources(const std::string& configPath);
    void createResourceDirectories();

    // Resource management
    void initialize();
    void shutdown();

    // Static path helpers
    static std::string getBasePath() {
        return getInstance().resolvePath("resources/");
    }

    static std::string getTexturePath(const std::string& category) {
        return getBasePath() + "textures/" + category + "/";
    }

    static std::string getMapPath(const std::string& mapId) {
        return getBasePath() + "maps/areas/" + mapId + ".json";
    }

    static std::string getMechanismPath(const std::string& type, const std::string& id) {
        return getBasePath() + "maps/mechanisms/" + type + "/" + id + ".json";
    }

    static std::string getAudioPath(const std::string& type) {
        return getBasePath() + "audio/" + type + "/";
    }

    // Config loading
    bool loadMapConfig(const std::string& mapId, nlohmann::json& outJson);
    bool loadMechanismConfig(const std::string& type,
        const std::string& id,
        nlohmann::json& outJson);

    // Audio resources management
    bool loadSound(const std::string& name, const std::string& path);
    void unloadSound(const std::string& name);

    irrklang::ISoundSource* getSound(const std::string& name) {
        auto it = m_sounds.find(name);
        if (it == m_sounds.end()) {
            DEBUG_LOG_ERROR("Sound not found: " << name);
            return nullptr;
        }
        return it != m_sounds.end() ? it->second : nullptr;
    }

private:
    ResourceManager() = default;
    ~ResourceManager() { shutdown(); }
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    struct PreloadConfig {
        std::vector<std::string> textures;
        std::vector<std::string> sounds;
        std::vector<std::string> maps;
    };

    std::string m_workingDirectory;
    std::unordered_map<std::string, std::unique_ptr<TextureData>> m_textures;
    std::unordered_map<std::string, irrklang::ISoundSource*> m_sounds;
    irrklang::ISoundEngine* m_soundEngine;

    // Path utilities
    std::string getExecutablePath() const;
    void ensureDirectoryExists(const std::string& path) const;

    // Texture loading utilities
    unsigned char* loadTextureData(const std::string& path, int& width, int& height, int& channels);
    void freeTextureData(unsigned char* data);
    bool loadJsonFile(const std::string& path, nlohmann::json& outJson);
    bool loadPreloadConfig(const std::string& configPath, PreloadConfig& config);
    GLenum getGLFormat(int channels);
};