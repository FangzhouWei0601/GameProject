#include "../../../../include/headers/map/mechanism/TriggerMechanism.h"

TriggerMechanism::TriggerMechanism(const std::string& id,
    const TriggerCondition& condition,
    const MechanismEffect& effect)
    : IMechanism(id, MechanismType::Trigger)
    , m_condition(condition)
    , m_effect(effect)
    , m_effectTimer(0.0f) {
}

void TriggerMechanism::activate() {
    if (m_state != MechanismState::Active) {
        m_state = MechanismState::Active;
        applyEffect();
    }
}

void TriggerMechanism::deactivate() {
    if (m_state == MechanismState::Active) {
        m_state = MechanismState::Inactive;
        removeEffect();
    }
}

void TriggerMechanism::update(float deltaTime) {
    if (m_state == MechanismState::Active) {
        // 更新效果持续时间
        if (m_effect.duration > 0) {
            m_effectTimer += deltaTime;
            if (m_effectTimer >= m_effect.duration) {
                deactivate();
                m_state = MechanismState::Finished;
                return;
            }
        }

        // 处理周期性效果
        if (m_effect.timing.isPeriodic) {
            applyPeriodicEffect(deltaTime);
        }

        // 处理移动效果
        if (m_effect.type == EffectType::MovePlatform) {
            handleMovementEffect(deltaTime);
        }

        // 检查触发条件是否仍然满足
        if (!checkConditions()) {
            deactivate();
        }
    }
}

void TriggerMechanism::reset() {
    deactivate();
    m_effectTimer = 0.0f;
    m_state = MechanismState::Inactive;
}

bool TriggerMechanism::checkConditions() {
    if (m_condition.customCondition && !m_condition.customCondition()) {
        return false;
    }

    // TODO: 添加玩家位置检查和交互检查
    return true;
}

void TriggerMechanism::applyEffect() {
    switch (m_effect.type) {
    case EffectType::Damage:
    case EffectType::Heal: {
        if (auto* target = findTarget(m_effect.targetId)) {
            float value = m_effect.type == EffectType::Damage ? -m_effect.value : m_effect.value;
            target->modifyHealth(value);
        }
        break;
    }

    case EffectType::Buff:
    case EffectType::Debuff: {
        if (auto* target = findTarget(m_effect.targetId)) {
            target->applyStatusEffect(m_effect);
        }
        break;
    }

    case EffectType::MovePlatform: {
        m_effect.movement.startPos = m_collider->getPosition();
        break;
    }

    case EffectType::Teleport: {
        if (auto* target = findTarget(m_effect.targetId)) {
            target->setPosition(m_effect.movement.endPos);
        }
        break;
    }
    }
}

void TriggerMechanism::removeEffect() {
    switch (m_effect.type) {
    case EffectType::Buff:
    case EffectType::Debuff: {
        if (auto* target = findTarget(m_effect.targetId)) {
            target->removeStatusEffect(m_effect);
        }
        break;
    }

    case EffectType::MovePlatform: {
        // 重置平台位置
        if (auto* collider = getCollider()) {
            collider->setPosition(m_effect.movement.startPos);
        }
        break;
    }
    }
}

void TriggerMechanism::applyPeriodicEffect(float deltaTime) {
    m_effect.timing.lastTriggerTime += deltaTime;
    if (m_effect.timing.lastTriggerTime >= m_effect.timing.interval) {
        applyEffect();
        m_effect.timing.lastTriggerTime = 0.0f;
    }
}

void TriggerMechanism::handleMovementEffect(float deltaTime) {
    if (auto* collider = getCollider()) {
        float totalDistance = glm::length(m_effect.movement.endPos - m_effect.movement.startPos);
        float progress = m_effectTimer * m_effect.movement.speed / totalDistance;

        if (m_effect.movement.isLoop) {
            progress = fmod(progress, 1.0f);
        }
        else {
            progress = std::min(progress, 1.0f);
        }

        glm::vec2 newPos = calculateMovementPosition(progress);
        collider->setPosition(newPos);
    }
}

glm::vec2 TriggerMechanism::calculateMovementPosition(float progress) {
    return m_effect.movement.startPos +
        (m_effect.movement.endPos - m_effect.movement.startPos) * progress;
}

void TriggerMechanism::registerTarget(const std::string& id, IEffectTarget* target) {
    if (target) {
        m_targets[id] = target;
    }
}

void TriggerMechanism::unregisterTarget(const std::string& id) {
    m_targets.erase(id);
}

IEffectTarget* TriggerMechanism::findTarget(const std::string& id) const {
    auto it = m_targets.find(id);
    return it != m_targets.end() ? it->second : nullptr;
}

