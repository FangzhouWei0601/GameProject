// include/headers/map/mechanism/TriggerMechanism.h
#pragma once
#include "IMechanism.h"
#include "IEffectTarget.h"
#include <unordered_map>

class TriggerMechanism : public IMechanism {
public:
    TriggerMechanism(const std::string& id,
        const TriggerCondition& condition,
        const MechanismEffect& effect);

    void activate() override;
    void deactivate() override;
    void update(float deltaTime) override;
    void reset() override;

    bool checkConditions();

    // 添加目标管理
    void registerTarget(const std::string& id, IEffectTarget* target);
    void unregisterTarget(const std::string& id);

    // 效果参数设置
    void setEffectValue(float value) { m_effect.value = value; }
    void setEffectDuration(float duration) { m_effect.duration = duration; }
    void setEffectDirection(const glm::vec2& direction) { m_effect.direction = direction; }

protected:
    IEffectTarget* findTarget(const std::string& id) const;

private:
    TriggerCondition m_condition;
    MechanismEffect m_effect;
    float m_effectTimer;

    std::unordered_map<std::string, IEffectTarget*> m_targets;

    void applyEffect();
    void removeEffect();
    void applyPeriodicEffect(float deltaTime);
    void handleMovementEffect(float deltaTime);
    glm::vec2 calculateMovementPosition(float progress);
};