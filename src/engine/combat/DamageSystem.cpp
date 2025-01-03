#include <random>
#include "../../../include/headers/combat/DamageSystem.h"

float DamageSystem::calculateDamage(const DamageInfo& damage, CombatStats& target) {
    // Hit Registration
    if (!checkHitSuccess(target.hitRate)) {
        return 0.0f;
    }

    float finalDamage = 0.0f;

    // Basic damage calculation
    if (damage.type == DamageType::Physical) {
        finalDamage = damage.amount * (100.0f / (100.0f + target.defense.physicalDefense));
    }
    else {
        finalDamage = damage.amount * (100.0f / (100.0f + target.defense.magicalDefense));
    }

    // Level Difference Correction
    int levelDiff = damage.attackerLevel - target.level;
    if (std::abs(levelDiff) >= 20) {
        finalDamage *= (levelDiff > 0) ? 1.5f : 0.3f;
    }
    else {
        finalDamage *= (1.0f + levelDiff * 0.015f);
    }

    // Critical Hit
    bool isCritical = checkCritical(target.criticalRate, target.defense.criticalResistance);
    if (isCritical) {
        finalDamage *= (target.criticalDamage / 100.0f);
    }

    return std::max(0.0f, finalDamage);
}

void DamageSystem::applyDamage(CombatStats& target, const DamageInfo& damage,
    const glm::vec2& direction) {
    if (target.isInvulnerable) return;

    // If in a dominant state, 
    // only damage is calculated, not knockback and stiffness.
    bool canBeInterrupted = (target.state != CombatState::SuperArmor);

    // Calculate and apply damage
    float finalDamage = calculateDamage(damage, target);
    target.currentHealth -= finalDamage;

    if (canBeInterrupted) {
        // Handle Knockback
        if (damage.canLaunch &&
            (1.0f - target.defense.launchResistance) > 0.0f) {
            target.state = CombatState::Launched;
            target.stateTimer = damage.hitstun;
        }
        // handle rigidity
        else {
            float stunDuration = damage.hitstun * (1.0f - target.defense.stunResistance);
            if (stunDuration > 0) {
                target.state = CombatState::Stunned;
                target.stateTimer = stunDuration;
            }
        }
    }

    // rest combo
    target.resetCombo();
}

bool DamageSystem::checkHitSuccess(float hitRate) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 100.0f);

    return dis(gen) <= std::clamp(hitRate, 60.0f, 100.0f);
}

bool DamageSystem::checkCritical(float criticalRate, float criticalResistance) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 100.0f);

    float finalRate = std::max(0.0f, criticalRate - criticalResistance);
    return dis(gen) <= finalRate;
}