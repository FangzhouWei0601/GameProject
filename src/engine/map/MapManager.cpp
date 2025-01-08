#include <nlohmann/json.hpp>
#include <fstream>
#include "../../../include/headers/map/MapManager.h"
#include "../../../include/headers/collision/BoxCollider.h"
#include "../../../include/headers/resource/ResourceManager.h"
#include "../../../include/headers/map/mechanism/TriggerMechanism.h"
#include "../../../include/headers/map/mechanism/SequenceMechanism.h"
#include <iostream>
#include "../../../include/headers/resource/ConfigValidator.h"
#include "../../../include/headers/map/portal/PortalSystem.h"
#include "../../../include/headers/map/LoadingScreen.h"
#include "../../../include/headers/map/mechanism/DoorMechanism.h"
#include "../../../include/headers/Engine.h"
#include "../../../include/headers/CommonDefines.h"

using json = nlohmann::json;

bool MapManager::loadAreaData(const std::string& filePath, AreaData& outData) {
    try {
        // 读取JSON文件
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;

        // 解析基本数据
        outData.id = j["id"].get<std::string>();
        outData.name = j["name"].get<std::string>();
        outData.type = static_cast<AreaType>(j["type"].get<int>());
        outData.isUnlocked = j["unlocked"].get<bool>();

        // 解析区域边界
        auto& bounds = j["bounds"];
        outData.bounds.position.x = bounds["x"].get<float>();
        outData.bounds.position.y = bounds["y"].get<float>();
        outData.bounds.size.x = bounds["width"].get<float>();
        outData.bounds.size.y = bounds["height"].get<float>();

        // 解析传送门
        if (j.contains("portals")) {
            for (const auto& portalJson : j["portals"]) {
                PortalData portal;
                portal.targetAreaId = portalJson["targetArea"].get<std::string>();
                portal.position.x = portalJson["x"].get<float>();
                portal.position.y = portalJson["y"].get<float>();
                portal.size.x = portalJson["width"].get<float>();
                portal.size.y = portalJson["height"].get<float>();
                portal.targetPosition.x = portalJson["targetX"].get<float>();
                portal.targetPosition.y = portalJson["targetY"].get<float>();
                portal.isLocked = portalJson["locked"].get<bool>();

                m_areas[outData.id]->addPortal(portal);
            }
        }

        if (j.contains("mechanisms")) {
            auto area = m_areas[outData.id].get();

            for (const auto& mechJson : j["mechanisms"]) {
                auto type = static_cast<MechanismType>(mechJson["type"].get<int>());
                std::string mechId = mechJson["id"].get<std::string>();

                std::unique_ptr<IMechanism> mechanism;

                switch (type) {
                case MechanismType::Trigger: {
                    // 创建触发机关
                    TriggerCondition condition;
                    condition.requiresPlayerPresence = mechJson["requiresPlayer"].get<bool>();
                    condition.triggerRadius = mechJson["radius"].get<float>();

                    MechanismEffect effect;
                    effect.type = static_cast<EffectType>(mechJson["effectType"].get<int>());
                    effect.targetId = mechJson["targetId"].get<std::string>();
                    effect.value = mechJson["value"].get<float>();
                    effect.duration = mechJson["duration"].get<float>();

                    mechanism = std::make_unique<TriggerMechanism>(mechId, condition, effect);
                    break;
                }

                case MechanismType::Sequence: {
                    // 创建序列机关
                    std::vector<std::string> sequence;
                    for (const auto& step : mechJson["sequence"]) {
                        sequence.push_back(step.get<std::string>());
                    }

                    auto seqMech = std::make_unique<SequenceMechanism>(mechId, sequence);
                    if (mechJson.contains("timeLimit")) {
                        seqMech->setTimeLimit(mechJson["timeLimit"].get<float>());
                    }

                    mechanism = std::move(seqMech);
                    break;
                }
                }

                // 设置机关的碰撞体
                if (mechJson.contains("collider")) {
                    auto& colliderJson = mechJson["collider"];
                    auto collider = std::make_unique<BoxCollider>(
                        glm::vec2(colliderJson["x"].get<float>(),
                            colliderJson["y"].get<float>()),
                        glm::vec2(colliderJson["width"].get<float>(),
                            colliderJson["height"].get<float>())
                    );
                    mechanism->setCollider(std::move(collider));
                }

                // 添加到区域
                area->addMechanism(std::move(mechanism));
            }
        }


        // 解析碰撞体
        if (j.contains("colliders")) {
            for (const auto& colliderJson : j["colliders"]) {
                auto collider = std::make_unique<BoxCollider>(
                    glm::vec2(colliderJson["x"].get<float>(), colliderJson["y"].get<float>()),
                    glm::vec2(colliderJson["width"].get<float>(), colliderJson["height"].get<float>())
                );
                m_areas[outData.id]->addCollider(std::move(collider));
            }
        }

        return true;
    }
    catch (const json::exception& e) {
        // 处理JSON解析错误
        return false;
    }
}

bool MapManager::loadArea(const std::string& areaId, const std::string& filePath) {
    std::cout << "Loading area: " << areaId << " from " << filePath << std::endl;
    // 检查是否已加载
    auto it = m_areas.find(areaId);
    if (it != m_areas.end()) {
        auto mechanisms = std::move(it->second->getMechanisms());

        if (!it->second->initializeRenderer()) {
            return false;
        }
        it->second->setMechanisms(std::move(mechanisms));
        return true;
    }


    AreaData areaData;
    nlohmann::json jsonData;
    nlohmann::json areaJson;

    if (!ResourceManager::getInstance().loadMapConfig(areaId, areaJson)) {
        std::cerr << "Failed to load map config for: " << areaId << std::endl;
        return false;
    }

    std::cout << "Parsing area data for: " << areaId << std::endl;
    if (!parseAreaData(areaJson, areaData)) {
        std::cerr << "Failed to parse area data for: " << areaId << std::endl;
        return false;
    }
    // 创建区域
    auto area = std::make_unique<Area>(areaData);

    if (!area->loadResources()) {
        std::cerr << "Failed to load resources for area: " << areaId << std::endl;
        return false;
    }

    if (areaJson.contains("portals")) {
        std::cout << "Loading portals for area: " << areaId << std::endl;
        for (const auto& portalJson : areaJson["portals"]) {
            try {
                PortalData portal;
                // 验证必要字段是否存在
                if (!portalJson.contains("x") || !portalJson.contains("y") ||
                    !portalJson.contains("width") || !portalJson.contains("height")) {
                    std::cout << "Portal data missing required fields, skipping..." << std::endl;
                    continue;
                }

                portal.targetAreaId = portalJson["targetArea"].get<std::string>();
                portal.position.x = portalJson["x"].get<float>();
                portal.position.y = portalJson["y"].get<float>();
                portal.size.x = portalJson["width"].get<float>();
                portal.size.y = portalJson["height"].get<float>();
                portal.targetPosition.x = portalJson["targetX"].get<float>();
                portal.targetPosition.y = portalJson["targetY"].get<float>();
                portal.isLocked = portalJson.value("locked", false);

                // 验证数据有效性
                if (portal.position.x < 0 || portal.position.y < 0 ||
                    portal.size.x <= 0 || portal.size.y <= 0) {
                    std::cout << "Invalid portal data detected, skipping..." << std::endl;
                    continue;
                }

                area->addPortal(portal);
            }
            catch (const std::exception& e) {
                std::cout << "Error loading portal: " << e.what() << std::endl;
                continue;
            }
        }
    }

    if (areaJson.contains("mechanisms")) {
        DEBUG_LOG("Loading mechanisms for area: " << areaId);
        for (const auto& mechData : areaJson["mechanisms"]) {
            auto type = static_cast<MechanismType>(mechData["type"].get<int>());
            auto id = mechData["id"].get<std::string>();

            // 检查机关ID是否已存在
            if (area->getMechanism(id) != nullptr) {
                DEBUG_LOG_WARN("Mechanism " << id << " already exists in area " << areaId);
                continue;
            }

            std::unique_ptr<IMechanism> mechanism = createMechanism(type, mechData);
            if (mechanism) {
                DEBUG_LOG("Created mechanism: " << id);
                area->addMechanism(std::move(mechanism));
            }
        }
    }

    if (areaJson.contains("portals")) {
        std::cout << "Loading portals for area: " << areaId << std::endl;
        for (const auto& portalJson : areaJson["portals"]) {
            PortalData portal;
            // ... 设置portal数据 ...
            area->addPortal(portal);
        }
        std::cout << "Area " << areaId << " now has "
            << area->getPortals().size() << " portals" << std::endl;
    }

    // 预加载区域资源
    if (!preloadAreaResources(areaId)) {
        return false;
    }

    
    if (!area->initializeRenderer()) {
        std::cerr << "Failed to initialize renderer for area: " << areaId << std::endl;
        return false;
    }

    // 加载机关和碰撞体
    loadMechanisms(area.get(), areaId);
    loadColliders(area.get(), jsonData);

    m_areas[areaId] = std::move(area);
    std::cout << "Successfully loaded area: " << areaId << std::endl;

    return true;
}

bool MapManager::parseAreaData(const nlohmann::json& json, AreaData& outData) {
    try {
        // 检查必需字段
        if (!json.contains("id") || !json.contains("name") ||
            !json.contains("type") || !json.contains("bounds")) {
            std::cerr << "Missing required fields in area data" << std::endl;
            return false;
        }

        outData.id = json["id"].get<std::string>();
        outData.name = json["name"].get<std::string>();
        outData.type = static_cast<AreaType>(json["type"].get<int>());
        outData.isUnlocked = json.value("unlocked", true);  // 默认为true

        // Parse bounds
        const auto& bounds = json["bounds"];
        if (!bounds.contains("x") || !bounds.contains("y") ||
            !bounds.contains("width") || !bounds.contains("height")) {
            std::cerr << "Missing required fields in bounds data" << std::endl;
            return false;
        }

        outData.bounds.position = {
            bounds["x"].get<float>(),
            bounds["y"].get<float>()
        };
        outData.bounds.size = {
            bounds["width"].get<float>(),
            bounds["height"].get<float>()
        };

        return true;
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "Area parsing error: " << e.what() << std::endl;
        return false;
    }
}

void MapManager::loadMechanisms(Area* area, const std::string& areaId) {
    if (!area) return;

    nlohmann::json mechJson;
    std::string mechPath = ResourceManager::getMechanismPath("triggers", areaId);

    // 检查机关是否已经加载
    if (area->getMechanisms().size() > 0) {
        DEBUG_LOG_WARN("Area " << areaId << " already has mechanisms loaded.");
        return;
    }

    // 如果区域没有机关配置文件，这是正常的
    if (!std::filesystem::exists(mechPath)) {
        DEBUG_LOG("No mechanism config found for area: " << areaId);
        return;
    }

    if (!ResourceManager::getInstance().loadMechanismConfig("triggers", areaId, mechJson)) {
        DEBUG_LOG_ERROR("Failed to load mechanism config for area: " << areaId);
        return;
    }

    for (const auto& mechData : mechJson["mechanisms"]) {
        auto type = static_cast<MechanismType>(mechData["type"].get<int>());
        auto id = mechData["id"].get<std::string>();

        // 检查机关ID是否已存在
        if (area->getMechanism(id) != nullptr) {
            DEBUG_LOG_WARN("Mechanism " << id << " already exists in area " << areaId);
            continue;
        }

        std::unique_ptr<IMechanism> mechanism = createMechanism(type, mechData);
        if (mechanism) {
            DEBUG_LOG("Adding mechanism " << id << " to area " << areaId);
            area->addMechanism(std::move(mechanism));
        }
    }
}

std::unique_ptr<IMechanism> MapManager::createMechanism(MechanismType type, const nlohmann::json& mechData) {
    try {
        std::string id = mechData["id"].get<std::string>();

        // 获取位置和大小
        glm::vec2 position(
            mechData["position"]["x"].get<float>(),
            mechData["position"]["y"].get<float>()
        );
        glm::vec2 size(
            mechData["size"]["x"].get<float>(),
            mechData["size"]["y"].get<float>()
        );

        std::unique_ptr<IMechanism> mechanism;

        if (mechData["id"].get<std::string>().find("door") != std::string::npos) {
            mechanism = std::make_unique<DoorMechanism>(id, position, size);
            if (mechanism) {
                auto collider = std::make_unique<BoxCollider>(position, size);
                // 门的碰撞设置
                collider->setCollisionLayer(static_cast<uint32_t>(CollisionLayerBits::Door));     // 0x4
                collider->setCollisionMask(static_cast<uint32_t>(CollisionLayerBits::Player));    // 0x1
                mechanism->setCollider(std::move(collider));
            }
        }
        else if (mechData["id"].get<std::string>().find("trigger") != std::string::npos) {
            TriggerCondition condition;
            condition.requiresPlayerPresence = mechData["requiresPlayer"].get<bool>();
            condition.triggerRadius = mechData["radius"].get<float>();

            MechanismEffect effect;
            effect.type = static_cast<EffectType>(mechData["effectType"].get<int>());
            effect.targetId = mechData["targetId"].get<std::string>();
            effect.value = mechData["value"].get<float>();
            effect.duration = mechData["duration"].get<float>();

            mechanism = std::make_unique<TriggerMechanism>(id, condition, effect);

            if (mechanism) {
                auto collider = std::make_unique<BoxCollider>(position, size);
                // 触发器的碰撞设置
                collider->setCollisionLayer(static_cast<uint32_t>(CollisionLayerBits::Trigger));  // 0x2
                collider->setCollisionMask(static_cast<uint32_t>(CollisionLayerBits::Player));    // 0x1
                mechanism->setCollider(std::move(collider));
            }
        }

        return mechanism;
    }
    catch (const nlohmann::json::exception& e) {
        DEBUG_LOG_ERROR("JSON error in createMechanism: " << e.what());
        return nullptr;
    }
}

bool MapManager::handlePortalTransition(const BoxCollider* playerCollider) {
    if (!m_currentArea || !playerCollider) return false;

    const auto& portals = m_currentArea->getPortals();

    PortalData triggeredPortal;
    if (PortalSystem::checkPortalTrigger(playerCollider, portals, triggeredPortal)) {
        return changeArea(triggeredPortal.targetAreaId, triggeredPortal.targetPosition);
    }

    return false;
}

bool MapManager::changeArea(const std::string& areaId, const glm::vec2& position) {
    if (m_isTransitioning) {
        DEBUG_LOG_WARN("Area transition already in progress");
        return false;
    }

    DEBUG_LOG("Changing area from " <<
        (m_currentArea ? m_currentArea->getId() : "None") <<
        " to " << areaId);

    if (auto* area = m_areas[areaId].get()) {
        auto bounds = area->getBounds();
        auto* camera = Renderer::getInstance().getCamera();
        camera->setBounds(bounds.position, bounds.position + bounds.size);
    }

    auto it = m_areas.find(areaId);
    if (it == m_areas.end()) {
        if (!loadArea(areaId, ResourceManager::getMapPath(areaId))) {
            DEBUG_LOG_ERROR("Failed to load area: " << areaId);
            return false;
        }
    }

    // 保存区域的机关状态
    auto area = m_areas[areaId].get();
    DEBUG_LOG("Area mechanisms count before transition: " << area->getMechanisms().size());

    // 重新初始化渲染器但保持机关状态
    if (!area->initializeRenderer()) {
        return false;
    }

    DEBUG_LOG("Area mechanisms count after renderer init: " << area->getMechanisms().size());

    m_currentArea = area;
    return true;
}

bool MapManager::preloadAreaResources(const std::string& areaId) {
    auto& resourceManager = ResourceManager::getInstance();

    // 尝试加载背景纹理
    std::string bgPath = ResourceManager::getTexturePath("backgrounds") + areaId + ".png";
    if (!std::filesystem::exists(bgPath)) {
        std::cout << "Using default background for area: " << areaId << std::endl;
        // 可以在这里加载一个默认的背景纹理
    }
    else {
        if (!resourceManager.loadTexture(areaId + "_bg", bgPath)) {
            return false;
        }
    }

    return true;
}

void MapManager::loadColliders(Area* area, const nlohmann::json& json) {
    if (!area || !json.contains("colliders")) return;

    try {
        for (const auto& colliderJson : json["colliders"]) {
            auto collider = std::make_unique<BoxCollider>(
                glm::vec2(colliderJson["x"].get<float>(),
                    colliderJson["y"].get<float>()),
                glm::vec2(colliderJson["width"].get<float>(),
                    colliderJson["height"].get<float>())
            );
            area->addCollider(std::move(collider));
        }
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "Error loading colliders: " << e.what() << std::endl;
    }
}

//void MapManager::update(float deltaTime) {
//    if (m_isTransitioning) {
//        m_transitionEffect->update(deltaTime);
//        m_loadingScreen->update(deltaTime);
//
//        if (m_transitionEffect->isFinished()) {
//            m_isTransitioning = false;
//            DEBUG_LOG("Area transition completed");
//        }
//    }
//
//    auto* playerCollider = Engine::getInstance().getPlayerCollider();
//    if (!playerCollider) {
//        DEBUG_LOG_ERROR("Player collider is null!");
//        return;
//    }
//
//    if (m_currentArea) {
//        m_currentArea->update(deltaTime);
//
//        DEBUG_LOG("当前区域机关数量: " << m_currentArea->getMechanisms().size());
//
//        for (auto& [id, mechanism] : m_currentArea->getMechanisms()) {
//            mechanism->update(deltaTime);
//
//            if (auto* trigger = dynamic_cast<TriggerMechanism*>(mechanism.get())) {
//                DEBUG_LOG("检查触发器 " << id);
//                DEBUG_LOG("目标门ID: " << trigger->getTargetId());
//
//                bool wasInRange = trigger->isActive();
//                bool isInRange = trigger->isPlayerInRange(playerCollider);
//
//                DEBUG_LOG("触发器状态: wasInRange=" << wasInRange << ", isInRange=" << isInRange);
//
//                if (isInRange && !wasInRange) {
//                    DEBUG_LOG("触发器被激活");
//                    auto* targetDoor = dynamic_cast<DoorMechanism*>(
//                        m_currentArea->getMechanism(trigger->getTargetId()));
//                    if (targetDoor) {
//                        DEBUG_LOG("找到目标门，尝试激活");
//                        targetDoor->activate();
//                    }
//                    else {
//                        DEBUG_LOG_ERROR("未找到目标门");
//                    }
//                }
//            }
//        }
//    }
//}

void MapManager::update(float deltaTime) {
    auto* playerCollider = Engine::getInstance().getPlayerCollider();
    if (!playerCollider) {
        return;
    }

    if (m_currentArea) {
        for (auto& [id, mechanism] : m_currentArea->getMechanisms()) {
            if (auto* trigger = dynamic_cast<TriggerMechanism*>(mechanism.get())) {
                bool wasInRange = trigger->isActive();
                bool isInRange = trigger->isPlayerInRange(playerCollider);

                DEBUG_LOG("触发器状态检查 - 之前状态:" << wasInRange << " 当前状态:" << isInRange);

                if (isInRange && !wasInRange) {
                    if (auto* targetDoor = dynamic_cast<DoorMechanism*>(
                        m_currentArea->getMechanism(trigger->getTargetId()))) {
                        DEBUG_LOG("激活门: " << trigger->getTargetId());
                        targetDoor->activate();
                    }
                }
            }
        }
    }
}

void MapManager::render() {
    if (m_currentArea) {
        m_currentArea->render();
    }

    if (m_isTransitioning) {
        m_transitionEffect->render();
        m_loadingScreen->render();
    }
}

void MapManager::startAreaTransition(const std::string& areaId, const glm::vec2& position) {
    m_isTransitioning = true;
    m_transitionEffect->start();
    m_loadingScreen->show();

    m_currentArea = m_areas[areaId].get();

    // Start async resource loading
    if (loadAreaResources(areaId)) {
        finalizeAreaChange(areaId, position);
    }
}

bool MapManager::loadAreaResources(const std::string& areaId) {
    m_loadingScreen->setProgress(0.0f);

    // Unload current area resources
    unloadCurrentArea();
    m_loadingScreen->setProgress(0.3f);

    // Load new area resources
    auto& area = m_areas[areaId];
    if (!area->loadResources()) {
        return false;
    }
    m_loadingScreen->setProgress(0.7f);

    // Initialize area systems
    if (!area->initializeRenderer()) {
        return false;
    }
    m_loadingScreen->setProgress(1.0f);

    return true;
}

void MapManager::unloadCurrentArea() {
    if (m_currentArea) {
        m_currentArea->unloadResources();
    }
}

void MapManager::finalizeAreaChange(const std::string& areaId, const glm::vec2& position) {
    // Set new current area
    m_currentArea = m_areas[areaId].get();

    // Reset player position
    // TODO: Add player position reset logic

    // Hide loading screen
    m_loadingScreen->hide();
    m_isTransitioning = false;
}