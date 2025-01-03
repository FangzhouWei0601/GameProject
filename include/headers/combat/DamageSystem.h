#pragma once
#include "CombatStats.h"
#include <glm/glm.hpp>

class DamageSystem {
public:
    // 计算伤害值
    static float calculateDamage(const DamageInfo& damage, CombatStats& target);

    // 应用伤害和效果
    static void applyDamage(CombatStats& target, const DamageInfo& damage,
        const glm::vec2& direction = glm::vec2(0.0f));

private:
    // 命中判定
    static bool checkHitSuccess(float hitRate);

    // 暴击判定
    static bool checkCritical(float criticalRate, float criticalResistance);
};