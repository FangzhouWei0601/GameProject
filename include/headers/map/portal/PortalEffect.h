#pragma once
#include <glm/glm.hpp>
#include "../../resource/TextureData.h"

class PortalEffect {
public:
    struct EffectProperties {
        float alpha = 1.0f;          // Transparency
        float scale = 1.0f;          // Effect scale
        float rotationSpeed = 45.0f;  // Degrees per second
        glm::vec4 color = glm::vec4(1.0f); // Effect color
    };

    PortalEffect();

    void update(float deltaTime);
    void render(const glm::vec2& position, const glm::vec2& size);

    // Effect control
    void play();
    void stop();
    void setActive(bool active);
    bool isActive() const { return m_isActive; }

    // Properties
    void setProperties(const EffectProperties& props) { m_properties = props; }
    const EffectProperties& getProperties() const { return m_properties; }

private:
    bool m_isActive;
    float m_currentRotation;
    float m_currentScale;
    float m_pulseTime;

    EffectProperties m_properties;

    void renderPortalGlow(const glm::vec2& position, const glm::vec2& size);
    void renderPortalRipple(const glm::vec2& position, const glm::vec2& size);
};