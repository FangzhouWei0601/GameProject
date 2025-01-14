#include "../../../../include/headers/map/portal/PortalEffect.h"
#include "../../include/headers/renderer/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

PortalEffect::PortalEffect()
    : m_isActive(false)
    , m_currentRotation(0.0f)
    , m_currentScale(1.0f)
    , m_pulseTime(0.0f) {
}

void PortalEffect::update(float deltaTime) {
    if (!m_isActive) return;

    // Update rotation
    m_currentRotation += m_properties.rotationSpeed * deltaTime;
    if (m_currentRotation >= 360.0f) {
        m_currentRotation -= 360.0f;
    }

    // Update pulse effect
    m_pulseTime += deltaTime;
    float pulseScale = 1.0f + 0.1f * sin(m_pulseTime * 2.0f);
    m_currentScale = m_properties.scale * pulseScale;
}

void PortalEffect::render(const glm::vec2& position, const glm::vec2& size) {
    if (!m_isActive) return;

    renderPortalGlow(position, size);
    renderPortalRipple(position, size);
}

void PortalEffect::renderPortalGlow(const glm::vec2& position, const glm::vec2& size) {
    RenderProperties props;
    props.position = position;
    props.size = size * m_currentScale;
    props.color = m_properties.color;
    props.color.a = m_properties.alpha;
    props.rotation = m_currentRotation;
    props.pivot = glm::vec2(0.5f);

    auto& renderer = Renderer::getInstance();
    renderer.drawRect(position, size, glm::vec3(props.color));
}

void PortalEffect::renderPortalRipple(const glm::vec2& position, const glm::vec2& size) {
    // Additional ripple effect rendering
    float rippleScale = 1.0f + 0.2f * sin(m_pulseTime * 3.0f);

    RenderProperties props;
    props.position = position;
    props.size = size * rippleScale;
    props.color = m_properties.color;
    props.color.a = m_properties.alpha * 0.5f;
    props.rotation = -m_currentRotation * 0.5f;
    props.pivot = glm::vec2(0.5f);

    auto& renderer = Renderer::getInstance();
    renderer.drawRect(position, size, glm::vec3(props.color));
}

void PortalEffect::play() {
    m_isActive = true;
    m_pulseTime = 0.0f;
}

void PortalEffect::stop() {
    m_isActive = false;
}

void PortalEffect::setActive(bool active) {
    if (active) {
        play();
    }
    else {
        stop();
    }
}