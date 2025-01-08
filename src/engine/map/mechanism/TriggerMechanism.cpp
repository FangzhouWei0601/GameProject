#include "../../../../include/headers/map/mechanism/TriggerMechanism.h"
#include "../../../../include/headers/renderer/Renderer.h"
#include <iostream>
#include "../../../../include/headers/Engine.h"
#include "../../../../include/headers/CommonDefines.h"

TriggerMechanism::TriggerMechanism(const std::string& id,
    const TriggerCondition& condition,
    const MechanismEffect& effect)
    : IMechanism(id, MechanismType::Trigger)
    , m_condition(condition)
    , m_effect(effect)
    , m_effectTimer(0.0f)
{
    // 创建默认碰撞体
    auto collider = std::make_unique<BoxCollider>(
        glm::vec2(0.0f),  // 默认位置
        glm::vec2(32.0f, 32.0f)  // 默认大小
    );

    // 设置碰撞层
    auto triggerLayer = CollisionManager::getInstance().getDefaultLayer(CollisionLayerBits::Trigger);
    collider->setCollisionLayer(triggerLayer.layer);
    collider->setCollisionMask(triggerLayer.mask);

    setCollider(std::move(collider));
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

    // 获取玩家位置
    auto* playerCollider = Engine::getInstance().getPlayerCollider();
    if (!playerCollider) return false;

    // 检查玩家是否在触发器范围内
    if (m_condition.triggerRadius > 0.0f) {
        glm::vec2 triggerCenter = getCollider()->getPosition() +
            getCollider()->getSize() * 0.5f;
        glm::vec2 playerCenter = playerCollider->getPosition() +
            playerCollider->getSize() * 0.5f;

        float distance = glm::length(triggerCenter - playerCenter);
        return distance <= m_condition.triggerRadius;
    }

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

void TriggerMechanism::render() {
    auto* collider = getCollider();
    if (!collider) return;

    // 使用固定的黄色
    auto& renderer = Renderer::getInstance();
    renderer.drawRect(
        collider->getPosition(),
        collider->getSize(),
        glm::vec3(1.0f, 1.0f, 0.0f)  // 纯黄色
    );

    //std::cout << "Trigger rendered at: "
    //    << collider->getPosition().x << ", "
    //    << collider->getPosition().y << std::endl;
}

bool TriggerMechanism::isPlayerInRange(const BoxCollider* playerCollider) const {
    auto* triggerCollider = getCollider();
    if (!playerCollider || !triggerCollider) {
        DEBUG_LOG("触发器碰撞体无效");
        return false;
    }

    DEBUG_LOG("触发器碰撞检查:");
    DEBUG_LOG("- 触发器ID: " << getId());
    DEBUG_LOG("- 触发器层: 0x" << std::hex << triggerCollider->getCollisionLayer().layer);
    DEBUG_LOG("- 触发器掩码: 0x" << std::hex << triggerCollider->getCollisionLayer().mask);
    DEBUG_LOG("- 玩家层: 0x" << std::hex << playerCollider->getCollisionLayer().layer);
    DEBUG_LOG("- 玩家掩码: 0x" << std::hex << playerCollider->getCollisionLayer().mask);

    // 确认碰撞层设置正确
    if (!(triggerCollider->getCollisionLayer().mask & playerCollider->getCollisionLayer().layer)) {
        DEBUG_LOG("Collision layer mismatch - Trigger layer: " <<
            std::hex << triggerCollider->getCollisionLayer().layer <<
            ", Player layer: " << playerCollider->getCollisionLayer().layer);
        return false;
    }

    DEBUG_LOG("检查触发器 " << getId() << " 与玩家的碰撞");
    DEBUG_LOG("触发器目标: " << m_effect.targetId);

    DEBUG_LOG("检查玩家和触发器碰撞");
    bool collision = triggerCollider->isColliding(playerCollider);
    if (collision) {
        DEBUG_LOG("触发器 " << getId() << " 被触发，目标门: " << m_effect.targetId);
    }
    return collision;
}

void TriggerMechanism::initializeCollider(const glm::vec2& position, const glm::vec2& size) {
    auto collider = std::make_unique<BoxCollider>(position, size);
    auto triggerLayer = CollisionManager::getInstance().getDefaultLayer(CollisionLayerBits::Trigger);
    collider->setCollisionLayer(triggerLayer.layer);
    collider->setCollisionMask(triggerLayer.mask);
    setCollider(std::move(collider));
}