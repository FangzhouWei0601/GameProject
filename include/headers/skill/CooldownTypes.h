#pragma once
#include <string>

struct CooldownModifier {
    float intelligenceReduction = 0.0f;  
    float equipmentReduction = 0.0f;     

    float getTotalReduction() const {
        float intReduction = std::min(intelligenceReduction, 30.0f);
        float equipReduction = std::min(equipmentReduction, 20.0f);
        return std::min(intReduction + equipReduction, 50.0f);
    }
};

struct CooldownInfo {
    float baseCooldown;         
    float currentCooldown;      
    bool isOnCooldown;         

    CooldownInfo(float base = 0.0f)
        : baseCooldown(base), currentCooldown(0.0f), isOnCooldown(false) {
    }
};