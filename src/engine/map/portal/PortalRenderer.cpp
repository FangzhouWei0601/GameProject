#include "../../../../include/headers/map/portal/PortalRenderer.h"
#include "../../../../include/headers/resource/ResourceManager.h"
#include "../../../../include/headers/renderer/Renderer.h"

PortalRenderer::PortalRenderer()
    : m_isActive(false)
    , m_baseColor(0.5f, 0.5f, 1.0f, 1.0f)
    , m_activeColor(0.0f, 1.0f, 1.0f, 1.0f)
    , m_portalTexture(nullptr) {
    m_effect = std::make_unique<PortalEffect>();
}

void PortalRenderer::initialize() {
    // Load portal textures
    m_portalTexture = ResourceManager::getInstance().getTexture("portal");

    // Setup default effect properties
    PortalEffect::EffectProperties props;
    props.color = m_activeColor;
    props.rotationSpeed = 30.0f;
    m_effect->setProperties(props);
}

void PortalRenderer::update(float deltaTime) {
    if (m_effect) {
        m_effect->update(deltaTime);
    }
}

void PortalRenderer::render(const PortalData& portal) {
    // Render base portal
    renderPortalBase(portal);

    // Render frame
    renderPortalFrame(portal);

    // Render effects if active
    if (m_isActive && m_effect) {
        m_effect->render(portal.position, portal.size);
    }
}

void PortalRenderer::renderPortalBase(const PortalData& portal) {
    RenderProperties props;
    props.position = portal.position;
    props.size = portal.size;
    props.color = m_isActive ? m_activeColor : m_baseColor;

    auto& renderer = Renderer::getInstance();
    if (m_portalTexture) {
        renderer.drawTexturedQuad(m_portalTexture, props);
    }
    else {
        renderer.drawRect(props.position, props.size, glm::vec3(props.color));
    }
}

void PortalRenderer::renderPortalFrame(const PortalData& portal) {
    // Render portal frame/border if needed
    RenderProperties props;
    props.position = portal.position;
    props.size = portal.size;
    props.color = glm::vec4(1.0f);

    // TODO: Add frame rendering if frame texture is available
}

void PortalRenderer::setActive(bool active) {
    m_isActive = active;
    if (m_effect) {
        m_effect->setActive(active);
    }
    updateEffectProperties();
}

void PortalRenderer::updateEffectProperties() {
    if (!m_effect) return;

    PortalEffect::EffectProperties props = m_effect->getProperties();
    props.color = m_isActive ? m_activeColor : m_baseColor;
    props.alpha = m_isActive ? 1.0f : 0.5f;
    m_effect->setProperties(props);
}