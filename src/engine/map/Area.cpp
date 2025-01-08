#include "../../../include/headers/map/Area.h"
#include "../../../include/headers/collision/BoxCollider.h"
#include "../../../include/headers/map/mechanism/IMechanism.h"
#include "../../../include/headers/resource/ResourceManager.h"
#include "../../../include/headers/map/BackgroundLayer.h"
#include "../../../include/headers/map/ObjectLayer.h"
#include "../../../include/headers/map/mechanism/DoorMechanism.h"
#include "../../../include/headers/map/mechanism/TriggerMechanism.h"
#include "../../../include/headers/CommonDefines.h"

Area::Area(const AreaData& data)
    : m_data(data) {
}

void Area::addPortal(const PortalData& portal) {
    // 验证传送门数据
    if (portal.position.x < 0 || portal.position.y < 0 ||
        portal.size.x <= 0 || portal.size.y <= 0) {
        std::cout << "Attempt to add invalid portal data, ignoring..." << std::endl;
        return;
    }

    std::cout << "Adding portal to area " << m_data.id
        << " Position: (" << portal.position.x << ", " << portal.position.y << ")"
        << " Size: (" << portal.size.x << ", " << portal.size.y << ")" << std::endl;

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
    static size_t lastMechanismCount = m_mechanisms.size();
    if (lastMechanismCount != m_mechanisms.size()) {
        DEBUG_LOG("Area " << m_data.id << " mechanism count changed to: " << m_mechanisms.size());
        lastMechanismCount = m_mechanisms.size();
    }

    for (auto& [id, mechanism] : m_mechanisms) {
        mechanism->update(deltaTime);
    }
}

void Area::render() {
    if (m_layerRenderer) {
        m_layerRenderer->render();

        // 渲染所有机关
        for (const auto& [id, mechanism] : m_mechanisms) {
            if (auto* door = dynamic_cast<DoorMechanism*>(mechanism.get())) {
                door->render();
            }
            else if (auto* trigger = dynamic_cast<TriggerMechanism*>(mechanism.get())) {
                trigger->render();
            }
        }
    }
}

void Area::renderMechanisms() {
    //std::cout << "Rendering mechanisms, count: " << m_mechanisms.size() << std::endl;
    for (const auto& [id, mechanism] : m_mechanisms) {
        //std::cout << "Mechanism ID: " << id << ", Type: " << static_cast<int>(mechanism->getType()) << std::endl;

        if (auto* door = dynamic_cast<DoorMechanism*>(mechanism.get())) {
            //std::cout << "Successfully cast to DoorMechanism" << std::endl;
            door->render();
        }
        else if (auto* trigger = dynamic_cast<TriggerMechanism*>(mechanism.get())) {
            //std::cout << "Successfully cast to TriggerMechanism" << std::endl;
            trigger->render();
        }
        else {
            std::cout << "Failed to cast mechanism to any known type" << std::endl;
        }
    }
}

void Area::addMechanism(std::unique_ptr<IMechanism> mechanism) {
    if (mechanism) {
        DEBUG_LOG("Adding mechanism " << mechanism->getId() << " to area " << m_data.id);
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

void Area::unloadResources() {
    // Clear the layer renderer
    if (m_layerRenderer) {
        m_layerRenderer->clear();
    }

    // Unload area-specific textures
    auto& resourceManager = ResourceManager::getInstance();
    resourceManager.unloadTexture(m_data.id + "_bg");

    // Clear mechanisms
    //m_mechanisms.clear();

    // Clear portals and colliders
    //m_portals.clear();
    m_colliders.clear();
}

bool Area::loadBackgroundTexture() {
    auto& resourceManager = ResourceManager::getInstance();

    // 修改为一致的文件名格式
    std::string textureName = m_data.id + "_bg";
    std::string texPath = ResourceManager::getTexturePath("backgrounds") + textureName + ".png";

    // 先检查是否已经加载
    if (resourceManager.getTexture(textureName)) {
        return true;  // 纹理已存在，直接返回
    }

    // 检查文件是否存在
    if (!std::filesystem::exists(texPath)) {
        // 只输出一次错误信息
        std::cout << "Using default background for area: " << m_data.id << std::endl;
        return true;  // 不视为错误，将使用纯色背景
    }

    // 加载纹理
    return resourceManager.loadTexture(textureName, texPath);
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

bool Area::initializeRenderer() {
    m_layerRenderer = std::make_unique<LayerRenderer>();

    // 添加调试信息
    std::cout << "Initializing renderer for area: " << m_data.id << std::endl;

    // background
    auto* bgTexture = ResourceManager::getInstance().getTexture(m_data.id + "_bg");
    if (bgTexture) {
        auto bgLayer = std::make_unique<BackgroundLayer>(bgTexture);
        bgLayer->setZOrder(0);
        bgLayer->setRepeat(true);
        bgLayer->setViewport(glm::vec2(0.0f), glm::vec2(800.0f, 600.0f));
        m_layerRenderer->addLayer(std::move(bgLayer));
    }
    else {
        std::cout << "No background texture found for: " << m_data.id + "_bg" << std::endl;
    }

    // object (moving platform)
    std::cout << "Begin initializing renderer, mechanism count: " << m_mechanisms.size() << std::endl;
    auto objectLayer = std::make_unique<ObjectLayer>();
    objectLayer->setZOrder(1);
    objectLayer->setViewport(glm::vec2(0.0f), glm::vec2(800.0f, 600.0f));

    //std::cout << "Adding " << m_portals.size() << " portals to object layer" << std::endl;

    std::cout << "Re-adding " << m_portals.size() << " portals to object layer" << std::endl;
    for (const auto& portal : m_portals) {
        objectLayer->addPortal(portal);
    }

    // collsion
    for (auto& collider : m_colliders) {
        objectLayer->addCollider(std::make_unique<BoxCollider>(*collider));
    }

    // portal
    for (const auto& portal : m_portals) {
        objectLayer->addPortal(portal);
    }

    m_layerRenderer->addLayer(std::move(objectLayer));

    std::cout << "End initializing renderer, mechanism count: " << m_mechanisms.size() << std::endl;
    return true;
}

void Area::setMechanisms(std::unordered_map<std::string, std::unique_ptr<IMechanism>>&& mechanisms) {
    m_mechanisms = std::move(mechanisms);
}