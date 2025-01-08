#include "../../../../include/headers/map/mechanism/DoorMechanism.h"
#include "../../../../include/headers/CommonDefines.h"

DoorMechanism::DoorMechanism(const std::string& id, const glm::vec2& position, const glm::vec2& size)
    : IMechanism(id, MechanismType::Door)
    , m_doorState(DoorState::Closed)
    , m_transitionProgress(0.0f)
    , m_transitionDuration(0.5f)
    , m_openSpeed(DOOR_OPEN_SPEED)
    , m_closeSpeed(DOOR_CLOSE_SPEED)
    , m_closedColor(0.8f, 0.2f, 0.2f, 1.0f)
    , m_openColor(0.2f, 0.8f, 0.2f, 0.5f)
    , m_lockedColor(0.5f, 0.1f, 0.1f, 1.0f)
{
    DEBUG_LOG("Creating door with size: (" << size.x << ", " << size.y << ") at position: ("
        << position.x << ", " << position.y << ")");
    auto collider = std::make_unique<BoxCollider>(position, size);
    auto doorLayer = CollisionManager::getInstance().getDefaultLayer(CollisionLayerBits::Door);
    collider->setCollisionLayer(doorLayer.layer);
    collider->setCollisionMask(doorLayer.mask);
    setCollider(std::move(collider));
}

void DoorMechanism::activate() {
    DEBUG_LOG("尝试激活门 " << getId());
    if (m_doorState == DoorState::Locked) {
        return;
    }

    if (m_doorState == DoorState::Closed || m_doorState == DoorState::Closing) {
        m_doorState = DoorState::Opening;
        m_state = MechanismState::Active;
        DEBUG_LOG("门已开始打开");
    }
}

void DoorMechanism::deactivate() {
    DEBUG_LOG("尝试关闭门 " << getId());
    if (m_doorState == DoorState::Locked) {
        return;
    }

    if (m_doorState == DoorState::Open || m_doorState == DoorState::Opening) {
        DEBUG_LOG("门正在关闭");
        m_doorState = DoorState::Closing;
        m_state = MechanismState::Inactive;
    }
}

void DoorMechanism::update(float deltaTime) {
    updateDoorState(deltaTime);
    updateCollider();
    updateVisuals();
}

void DoorMechanism::reset()
{
}

void DoorMechanism::updateDoorState(float deltaTime) {
    float targetProgress = 0.0f;
    float speed = 0.0f;

    switch (m_doorState) {
    case DoorState::Opening:
        targetProgress = 1.0f;
        speed = m_openSpeed;
        break;
    case DoorState::Closing:
        targetProgress = 0.0f;
        speed = m_closeSpeed;
        break;
    default:
        return;
    }

    // 平滑过渡动画
    float step = speed * deltaTime;
    if (std::abs(targetProgress - m_transitionProgress) <= step) {
        m_transitionProgress = targetProgress;
        m_doorState = (targetProgress >= 1.0f) ? DoorState::Open : DoorState::Closed;
    }
    else {
        m_transitionProgress += (targetProgress > m_transitionProgress) ? step : -step;
    }
}

void DoorMechanism::updateCollider() {
    auto* collider = getCollider();
    if (!collider) return;

    auto doorLayer = CollisionManager::getInstance().getDefaultLayer(CollisionLayerBits::Door);

    if (m_doorState == DoorState::Open ||
        (m_doorState == DoorState::Opening && m_transitionProgress >= COLLISION_DISABLE_THRESHOLD)) {
        collider->setCollisionLayer(static_cast<uint32_t>(CollisionLayerBits::None));
        collider->setCollisionMask(0);
        //DEBUG_LOG("Door " << getId() << " collision disabled");
    }
    else {
        collider->setCollisionLayer(doorLayer.layer);
        collider->setCollisionMask(doorLayer.mask);
        //DEBUG_LOG("Door " << getId() << " collision enabled");
    }
}

void DoorMechanism::updateVisuals() {
    // 可以添加粒子效果或其他视觉反馈
}

glm::vec4 DoorMechanism::getCurrentColor() const {
    if (m_doorState == DoorState::Locked) {
        return m_lockedColor;
    }
    return glm::mix(m_closedColor, m_openColor, m_transitionProgress);
}

void DoorMechanism::setLocked(bool locked) {
    if (locked && m_doorState != DoorState::Locked) {
        m_doorState = DoorState::Locked;
        m_transitionProgress = 0.0f;
        updateCollider();
    }
    else if (!locked && m_doorState == DoorState::Locked) {
        m_doorState = DoorState::Closed;
    }
}

void DoorMechanism::render() {
    auto* collider = getCollider();
    if (!collider) return;

    
    glm::vec4 color = getCurrentColor();
    auto& renderer = Renderer::getInstance();

    // 绘制门的主体
    renderer.drawRect(
        collider->getPosition(),
        collider->getSize(),
        glm::vec3(color),
        color.a
    );

    // 如果是过渡状态，可以添加额外的视觉效果
    if (m_doorState == DoorState::Opening || m_doorState == DoorState::Closing) {
        // 添加过渡动画效果
        float effectIntensity = std::sin(m_transitionProgress * glm::pi<float>());
        glm::vec4 effectColor(1.0f, 1.0f, 1.0f, effectIntensity * 0.3f);

        renderer.drawRect(
            collider->getPosition(),
            collider->getSize(),
            glm::vec3(effectColor),
            effectColor.a
        );
    }
}