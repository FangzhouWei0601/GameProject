#pragma once
#include "PortalEffect.h"
#include "../MapTypes.h"
#include "../../resource/TextureData.h"
#include <memory>

class PortalRenderer {
public:
    PortalRenderer();

    void initialize();
    void update(float deltaTime);
    void render(const PortalData& portal);

    // Visual state
    void setActive(bool active);
    bool isActive() const { return m_isActive; }

    // Appearance
    void setBaseColor(const glm::vec4& color) { m_baseColor = color; }
    void setActiveColor(const glm::vec4& color) { m_activeColor = color; }

private:
    bool m_isActive;
    glm::vec4 m_baseColor;
    glm::vec4 m_activeColor;

    std::unique_ptr<PortalEffect> m_effect;
    TextureData* m_portalTexture;

    void renderPortalBase(const PortalData& portal);
    void renderPortalFrame(const PortalData& portal);
    void updateEffectProperties();
};