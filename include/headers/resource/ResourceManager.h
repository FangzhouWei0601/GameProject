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

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    struct ResourceError {
        std::string message;
        std::string resourceName;
        std::string resourcePath;
    };

    const std::vector<ResourceError>& getErrors() const { return m_errors; }
    void clearErrors() { m_errors.clear(); }

    // Texture management
    bool loadTexture(const std::string& name, const std::string& path);
    void unloadTexture(const std::string& name);
    TextureData* getTexture(const std::string& name);

    bool preloadResources(const std::string& configPath);
    void createResourceDirectories();

    // Resource management
    void initialize();
    void shutdown();


    // path management
    static std::string getBasePath() {
        return "resources/";
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

    // mapconfig
    bool loadMapConfig(const std::string& mapId, nlohmann::json& outJson);
    bool loadMechanismConfig(const std::string& type,
        const std::string& id,
        nlohmann::json& outJson);

    // audio
    static std::string getAudioPath(const std::string& type) {
        return getBasePath() + "audio/" + type + "/";
    }

    // audio resources management
    bool loadSound(const std::string& name, const std::string& path);
    void unloadSound(const std::string& name);

    irrklang::ISoundSource* getSound(const std::string& name) {
        auto it = m_sounds.find(name);
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

    std::unordered_map<std::string, std::unique_ptr<TextureData>> m_textures;

    std::unordered_map<std::string, irrklang::ISoundSource*> m_sounds;
    irrklang::ISoundEngine* m_soundEngine;

    // Texture loading utilities
    unsigned char* loadTextureData(const std::string& path, int& width, int& height, int& channels);
    void freeTextureData(unsigned char* data);
    bool loadJsonFile(const std::string& path, nlohmann::json& outJson);

    bool loadPreloadConfig(const std::string& configPath, PreloadConfig& config);

    GLenum getGLFormat(int channels);

    std::vector<ResourceError> m_errors;
    void logError(const std::string& message,
        const std::string& resourceName = "",
        const std::string& resourcePath = "");
};