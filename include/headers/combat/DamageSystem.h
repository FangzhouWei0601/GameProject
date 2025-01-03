#pragma once
#include "CombatStats.h"
#include <glm/glm.hpp>

class DamageSystem {
public:
    // �����˺�ֵ
    static float calculateDamage(const DamageInfo& damage, CombatStats& target);

    // Ӧ���˺���Ч��
    static void applyDamage(CombatStats& target, const DamageInfo& damage,
        const glm::vec2& direction = glm::vec2(0.0f));

private:
    // �����ж�
    static bool checkHitSuccess(float hitRate);

    // �����ж�
    static bool checkCritical(float criticalRate, float criticalResistance);
};