#pragma once
#include "MechanismTypes.h"
#include <glm/glm.hpp>

class IEffectTarget {
public:
    virtual ~IEffectTarget() = default;

    virtual void modifyHealth(float amount) = 0;
    virtual void applyStatusEffect(const MechanismEffect& effect) = 0;
    virtual void removeStatusEffect(const MechanismEffect& effect) = 0;
    virtual void setPosition(const glm::vec2& position) = 0;
};