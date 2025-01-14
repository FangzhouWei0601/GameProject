#pragma once
#include "CooldownTypes.h"
#include "../character/AttributeSystem.h"
#include <unordered_map>

class CooldownSystem {
public:
    CooldownSystem(AttributeSystem& attributes);

    void registerSkill(const std::string& skillId, float baseCooldown);

    void triggerCooldown(const std::string& skillId);

    bool isOnCooldown(const std::string& skillId) const;
    float getRemainingCooldown(const std::string& skillId) const;

    void update(float deltaTime);

    void addEquipmentCDR(float reduction);
    void removeEquipmentCDR(float reduction);

    const CooldownModifier& getCooldownModifier() const { return m_modifier; }

private:
    AttributeSystem& m_attributes;
    CooldownModifier m_modifier;
    std::unordered_map<std::string, CooldownInfo> m_cooldowns;

    void updateIntelligenceCDR();

    float calculateActualCooldown(float baseCooldown) const;
};