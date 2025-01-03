#pragma once
#include "CombatTypes.h"
#include <chrono>

class CombatStats {
public:
    // basic attribute
    float maxHealth = 100.0f;
    float currentHealth = 100.0f;
    float maxMana = 100.0f;
    float currentMana = 100.0f;
    int level = 1;

    // attack attribute
    float physicalAttack = 10.0f;
    float magicalAttack = 10.0f;
    float hitRate = 90.0f;         // hit rate
    float criticalRate = 5.0f;     
    float criticalDamage = 150.0f; // critical damage percent

    // defense
    DefenseInfo defense;

    // stat
    CombatState state = CombatState::Normal;
    float stateTimer = 0.0f;      // stat time
    bool isInvulnerable = false;

    // combo
    int comboCount = 0;
    float comboTimer = 0.0f;
    static constexpr float COMBO_TIMEOUT = 10.0f; // combo over

    void update(float deltaTime) {
        // update stat timer
        if (stateTimer > 0) {
            stateTimer -= deltaTime;
            if (stateTimer <= 0) {
                state = CombatState::Normal;
            }
        }

        // update combo timer
        if (comboCount > 0) {
            comboTimer += deltaTime;
            if (comboTimer >= COMBO_TIMEOUT) {
                resetCombo();
            }
        }
    }

    void increaseCombo() {
        comboCount++;
        comboTimer = 0.0f;
    }

    void resetCombo() {
        comboCount = 0;
        comboTimer = 0.0f;
    }

    ComboRank getComboRank() const {
        if (comboCount == 0) return ComboRank::None;
        if (comboCount <= 10) return ComboRank::C;
        if (comboCount <= 20) return ComboRank::B;
        if (comboCount <= 30) return ComboRank::A;
        if (comboCount <= 50) return ComboRank::S;
        return ComboRank::SP;
    }
};