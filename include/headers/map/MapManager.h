#pragma once
#include "Area.h"
#include <unordered_map>
#include <memory>
#include "AreaTransitionEffect.h"
#include "LoadingScreen.h"
#include <iostream>

class MapManager {
public:
    static MapManager& getInstance() {
        static MapManager instance;
        return instance;
    }

    bool loadArea(const std::string& areaId, const std::string& filePath);
    bool parseAreaData(const nlohmann::json& json, AreaData& outData);
    void unloadArea(const std::string& areaId);
    Area* getCurrentArea() const { return m_currentArea; }
    Area* getArea(const std::string& areaId) const;

    bool changeArea(const std::string& areaId, const glm::vec2& position);
    bool handlePortalTransition(const BoxCollider* playerCollider);

    void update(float deltaTime);
    void render();

    void loadMechanisms(Area* area, const std::string& areaId);

private:
    MapManager()
        : m_currentArea(nullptr)
        , m_transitionEffect(std::make_unique<AreaTransitionEffect>())
        , m_loadingScreen(std::make_unique<LoadingScreen>())
        , m_isTransitioning(false) {
    }
    ~MapManager() = default;
    MapManager(const MapManager&) = delete;
    MapManager& operator=(const MapManager&) = delete;

    std::unordered_map<std::string, std::unique_ptr<Area>> m_areas;
    std::unique_ptr<AreaTransitionEffect> m_transitionEffect;
    std::unique_ptr<LoadingScreen> m_loadingScreen;
    bool m_isTransitioning = false;
    Area* m_currentArea;

    std::unique_ptr<IMechanism> createMechanism(MechanismType type, const nlohmann::json& mechData);

    bool loadAreaData(const std::string& filePath, AreaData& outData);
    bool preloadAreaResources(const std::string& areaId);
    void loadColliders(Area* area, const nlohmann::json& json);

    void startAreaTransition(const std::string& areaId, const glm::vec2& position);
    bool loadAreaResources(const std::string& areaId);
    void unloadCurrentArea();
    void finalizeAreaChange(const std::string& areaId, const glm::vec2& position);
    std::string getAreaPath(const std::string& areaId) const;

    //std::unique_ptr<IMechanism> createTriggerMechanism(const nlohmann::json& mechData);
    //std::unique_ptr<IMechanism> createSequenceMechanism(const nlohmann::json& mechData);
};