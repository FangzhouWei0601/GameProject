#include "../../../include/headers/camera/Camera.h"

Camera::Camera(float width, float height)
    : m_position(0.0f)
    , m_zoom(1.0f)
    , m_screenSize(width, height)
    , m_boundsMin(-std::numeric_limits<float>::max())
    , m_boundsMax(std::numeric_limits<float>::max())
{
    updateMatrices();
}

void Camera::setPosition(const glm::vec2& pos) {
    m_position = clampPosition(pos);
    updateMatrices();
}

void Camera::setZoom(float zoom) {
    m_zoom = glm::clamp(zoom, 0.1f, 10.0f);
    updateMatrices();
}

void Camera::setBounds(const glm::vec2& min, const glm::vec2& max) {
    m_boundsMin = min;
    m_boundsMax = max;
    m_position = clampPosition(m_position);
    updateMatrices();
}

void Camera::update() {
    updateMatrices();
}

void Camera::updateMatrices() {
    float halfWidth = m_screenSize.x * 0.5f / m_zoom;
    float halfHeight = m_screenSize.y * 0.5f / m_zoom;
    m_projectionMatrix = glm::ortho(-halfWidth, halfWidth, halfHeight, -halfHeight);

    m_viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-m_position, 0.0f));
}

glm::vec2 Camera::clampPosition(const glm::vec2& pos) const {
    return glm::clamp(pos, m_boundsMin, m_boundsMax);
}

void Camera::setTarget(const glm::vec2& target) {
    m_targetPosition = target;
    m_hasTarget = true;
}

void Camera::updateFollow(float deltaTime) {
    if (!m_hasTarget) return;

    glm::vec2 toTarget = m_targetPosition - m_position;
    if (glm::length(toTarget) > 0.1f) {
        m_position += toTarget * m_followSpeed * deltaTime;
        m_position = clampPosition(m_position);
        updateMatrices();
    }
}