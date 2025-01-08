#pragma once
#include "IMechanism.h"
#include "IEffectTarget.h"
#include <unordered_map>

class TriggerMechanism : public IMechanism {
public:
    TriggerMechanism(const std::string& id,
        const TriggerCondition& condition,
        const MechanismEffect& effect);

    const std::string& getTargetId() const { return m_effect.targetId; }

    void activate() override;
    void deactivate() override;
    void update(float deltaTime) override;
    void reset() override;
    void render();
    void initializeCollider(const glm::vec2& position, const glm::vec2& size);
    bool isPlayerInRange(const BoxCollider* playerCollider) const;
    bool checkConditions();

    // 添加目标管理
    void registerTarget(const std::string& id, IEffectTarget* target);
    void unregisterTarget(const std::string& id);

    // 效果参数设置
    void setEffectValue(float value) { m_effect.value = value; }
    void setEffectDuration(float duration) { m_effect.duration = duration; }
    void setEffectDirection(const glm::vec2& direction) { m_effect.direction = direction; }

    bool isActive() const { return m_state == MechanismState::Active; }
    //const std::string& getTargetId() const { return m_effect.targetId; }

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