#include <nlohmann/json.hpp>
#include <fstream>
#include "../../../include/headers/map/MapManager.h"
#include "../../../include/headers/collision/BoxCollider.h"
#include "../../../include/headers/resource/ResourceManager.h"
//#include "../../../include/headers/map/mechanism/IMechanism.h"
#include "../../../include/headers/map/mechanism/TriggerMechanism.h"
#include "../../../include/headers/map/mechanism/SequenceMechanism.h"
#include <iostream>

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
    AreaData areaData;
    nlohmann::json jsonData;

    if (!ResourceManager::getInstance().loadMapConfig(areaId, jsonData)) {
        return false;
    }

    // 解析区域数据
    if (!parseAreaData(jsonData, areaData)) {
        return false;
    }

    // 创建并存储区域
    m_areas[areaId] = std::make_unique<Area>(areaData);
    return true;
}

bool MapManager::parseAreaData(const nlohmann::json& json, AreaData& outData) {
    try {
        outData.id = json["id"].get<std::string>();
        outData.name = json["name"].get<std::string>();
        outData.type = static_cast<AreaType>(json["type"].get<int>());
        outData.isUnlocked = json["unlocked"].get<bool>();

        // Parse bounds
        auto& bounds = json["bounds"];
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
