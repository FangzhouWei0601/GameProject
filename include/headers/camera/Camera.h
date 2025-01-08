#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float width, float height);

    void setPosition(const glm::vec2& pos);
    void setZoom(float zoom);
    void setBounds(const glm::vec2& min, const glm::vec2& max);

    const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    const glm::vec2& getPosition() const { return m_position; }
    float getZoom() const { return m_zoom; }

    void setTarget(const glm::vec2& target);
    void setFollowSpeed(float speed) { m_followSpeed = speed; }
    void updateFollow(float deltaTime);

    void update();

private:
    glm::vec2 m_position;
    float m_zoom;
    glm::vec2 m_screenSize;

    glm::vec2 m_boundsMin;
    glm::vec2 m_boundsMax;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    void updateMatrices();
    glm::vec2 clampPosition(const glm::vec2& pos) const;

    glm::vec2 m_targetPosition;
    float m_followSpeed = 5.0f;
    bool m_hasTarget = false;
};