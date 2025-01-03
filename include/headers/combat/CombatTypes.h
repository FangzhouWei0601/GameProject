#pragma once
#include <glm/glm.hpp>

enum class DamageType {
    Physical,
    Magical
};

enum class CombatState {
    Normal,
    SuperArmor,    
    Stunned,       
    Launched,      
    Downed        
};

enum class ComboRank {
    None,
    C,  // 1-10 hits
    B,  // 11-20 hits
    A,  // 21-30 hits
    S,  // 31-50 hits
    SP  // 50+ hits
};

struct DamageInfo {
    float amount = 0.0f;
    DamageType type = DamageType::Physical;
    float knockback = 0.0f;        // knockback power
    float hitstun = 0.0f;          // rigidity time
    bool canLaunch = false;        // if knockback
    int attackerLevel = 1;         // attacker level
    bool isCritical = false;       // if critical hit
};

struct DefenseInfo {
    float physicalDefense = 0.0f;
    float magicalDefense = 0.0f;
    float knockbackResistance = 0.0f;
    float launchResistance = 0.0f;
    float downedResistance = 0.0f;
    float stunResistance = 0.0f;
    float criticalResistance = 0.0f;
};