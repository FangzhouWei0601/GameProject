#pragma once
#include "MapTypes.h"
#include "../../../include/headers/collision/BoxCollider.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include "../../../include/headers/map/mechanism/IMechanism.h"
#include "LayerRenderer.h"

class Area {
public:
    Area(const AreaData& data);
    virtual ~Area() = default;

    const std::string& getId() const { return m_data.id; }
    const std::string& getName() const { return m_data.name; }
    AreaType getType() const { return m_data.type; }
    const AreaBounds& getBounds() const { return m_data.bounds; }

    bool isDiscovered() const { return m_data.isDiscovered; }
    bool isUnlocked() const { return m_data.isUnlocked; }
    void setDiscovered(bool discovered) { m_data.isDiscovered = discovered; }
    void setUnlocked(bool unlocked) { m_data.isUnlocked = unlocked; }

    void addPortal(const PortalData& portal);
    const std::vector<PortalData>& getPortals() const { return m_portals; }
    bool hasPortalTo(const std::string& areaId) const;

    void addCollider(std::unique_ptr<BoxCollider> collider);
    const std::vector<std::unique_ptr<BoxCollider>>& getColliders() const { return m_colliders; }

    virtual void update(float deltaTime);
    virtual void render();
    void renderMechanisms();

    bool containsPoint(const glm::vec2& point) const {
        return m_data.bounds.containsPoint(point);
    }

    void addMechanism(std::unique_ptr<IMechanism> mechanism);
    IMechanism* getMechanism(const std::string& id);
    void activateMechanismInRange(const glm::vec2& position, float radius);
    void updateMechanisms(float deltaTime);

    bool loadResources();
    void unloadResources();
    bool initializeRenderer();

    std::unordered_map<std::string, std::unique_ptr<IMechanism>>& getMechanisms() { return m_mechanisms; }
    void setMechanisms(std::unordered_map<std::string, std::unique_ptr<IMechanism>>&& mechanisms);

private:
    std::unordered_map<std::string, std::unique_ptr<IMechanism>> m_mechanisms;
    std::unique_ptr<LayerRenderer> m_layerRenderer;

    bool loadBackgroundTexture();
    bool loadMechanismConfigs();


protected:
    AreaData m_data;
    std::vector<PortalData> m_portals;
    std::vector<std::unique_ptr<BoxCollider>> m_colliders;
};