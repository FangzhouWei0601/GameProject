#include "../../../include/headers/map/Area.h"
#include "../../../include/headers/collision/BoxCollider.h"
#include "../../../include/headers/map/mechanism/IMechanism.h"
#include "../../../include/headers/resource/ResourceManager.h"

Area::Area(const AreaData& data)
    : m_data(data) {
}

void Area::addPortal(const PortalData& portal) {
    m_portals.push_back(portal);
}

bool Area::hasPortalTo(const std::string& areaId) const {
    for (const auto& portal : m_portals) {
        if (portal.targetAreaId == areaId) {
            return true;
        }
    }
    return false;
}

void Area::addCollider(std::unique_ptr<BoxCollider> collider) {
    if (collider) {
        m_colliders.push_back(std::move(collider));
    }
}

void Area::update(float deltaTime) {
    // Update dynamic elements within a region
}

void Area::render() {
    // Rendering Region Contents
}

void Area::addMechanism(std::unique_ptr<IMechanism> mechanism) {
    if (mechanism) {
        m_mechanisms[mechanism->getId()] = std::move(mechanism);
    }
}

IMechanism* Area::getMechanism(const std::string& id) {
    auto it = m_mechanisms.find(id);
    return it != m_mechanisms.end() ? it->second.get() : nullptr;
}

void Area::activateMechanismInRange(const glm::vec2& position, float radius) {
    float radiusSquared = radius * radius;

    for (auto& [id, mechanism] : m_mechanisms) {
        if (auto* collider = mechanism->getCollider()) {
            glm::vec2 mechanismPos = collider->getPosition();

            // 或者使用 glm::length 计算向量长度
            float distanceSquared = glm::length(position - mechanismPos);
            distanceSquared *= distanceSquared;

            if (distanceSquared <= radiusSquared) {
                mechanism->activate();
            }
        }
    }
}

void Area::updateMechanisms(float deltaTime) {
    for (auto& [id, mechanism] : m_mechanisms) {
        mechanism->update(deltaTime);
    }
}

bool Area::loadResources() {
    if (!loadBackgroundTexture()) return false;
    if (!loadMechanismConfigs()) return false;
    return true;
}

bool Area::loadBackgroundTexture() {
    std::string texPath = ResourceManager::getTexturePath("backgrounds") + m_data.id + ".png";
    return ResourceManager::getInstance().loadTexture(m_data.id + "_bg", texPath);
}

bool Area::loadMechanismConfigs() {
    nlohmann::json mechJson;
    if (!ResourceManager::getInstance().loadMechanismConfig("triggers", m_data.id, mechJson)) {
        return false;
    }

    for (const auto& mech : mechJson["mechanisms"]) {
        // 机关配置解析代码
    }

    return true;
}