#include <algorithm>
#include "../../../include/headers/skill/CooldownSystem.h"
#include "../../../include/headers/character/AttributeSystem.h"

CooldownSystem::CooldownSystem(AttributeSystem& attributes)
    : m_attributes(attributes) {
    updateIntelligenceCDR();
}

void CooldownSystem::registerSkill(const std::string& skillId, float baseCooldown) {
    m_cooldowns[skillId] = CooldownInfo(baseCooldown);
}

void CooldownSystem::triggerCooldown(const std::string& skillId) {
    auto it = m_cooldowns.find(skillId);
    if (it != m_cooldowns.end()) {
        it->second.currentCooldown = calculateActualCooldown(it->second.baseCooldown);
        it->second.isOnCooldown = true;
    }
}

bool CooldownSystem::isOnCooldown(const std::string& skillId) const {
    auto it = m_cooldowns.find(skillId);
    return it != m_cooldowns.end() && it->second.isOnCooldown;
}

float CooldownSystem::getRemainingCooldown(const std::string& skillId) const {
    auto it = m_cooldowns.find(skillId);
    return (it != m_cooldowns.end()) ? it->second.currentCooldown : 0.0f;
}

void CooldownSystem::update(float deltaTime) {
    updateIntelligenceCDR();

    for (auto& [skillId, cooldown] : m_cooldowns) {
        if (cooldown.isOnCooldown) {
            cooldown.currentCooldown -= deltaTime;
            if (cooldown.currentCooldown <= 0.0f) {
                cooldown.currentCooldown = 0.0f;
                cooldown.isOnCooldown = false;
            }
        }
    }
}

void CooldownSystem::addEquipmentCDR(float reduction) {
    m_modifier.equipmentReduction += reduction;
}

void CooldownSystem::removeEquipmentCDR(float reduction) {
    m_modifier.equipmentReduction = std::max(0.0f,
        m_modifier.equipmentReduction - reduction);
}

void CooldownSystem::updateIntelligenceCDR() {
    int intelligence = m_attributes.getTotalAttribute(AttributeType::Intelligence);
    m_modifier.intelligenceReduction = std::floor(intelligence / 25.0f);
}

float CooldownSystem::calculateActualCooldown(float baseCooldown) const {
    float reduction = m_modifier.getTotalReduction() / 100.0f;
    return baseCooldown * (1.0f - reduction);
}