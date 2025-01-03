#pragma once
#include "Area.h"
#include <unordered_map>
#include <memory>

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

    bool usePortal(const PortalData& portal);

    void update(float deltaTime);
    void render();

private:
    MapManager() : m_currentArea(nullptr) {}
    ~MapManager() = default;
    MapManager(const MapManager&) = delete;
    MapManager& operator=(const MapManager&) = delete;

    std::unordered_map<std::string, std::unique_ptr<Area>> m_areas;
    Area* m_currentArea;

    bool loadAreaData(const std::string& filePath, AreaData& outData);
};