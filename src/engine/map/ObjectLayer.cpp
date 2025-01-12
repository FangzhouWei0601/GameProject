#include "../../../include/headers/map/ObjectLayer.h"
#include "../../../include/headers/renderer/Renderer.h"
#include "../../../include/headers/resource/ResourceManager.h"

void ObjectLayer::update(float deltaTime) {
    // todo
}

void ObjectLayer::render() {
    renderColliders();
    renderPortals();
}

void ObjectLayer::addCollider(std::unique_ptr<BoxCollider> collider) {
    if (collider) {
        m_colliders.push_back(std::move(collider));
    }
}

void ObjectLayer::addPortal(const PortalData& portal) {
    m_portals.push_back(portal);
}

void ObjectLayer::renderColliders() {
    auto& renderer = Renderer::getInstance();

    // Only render colliders in debug mode
#ifdef _DEBUG
    for (const auto& collider : m_colliders) {
        RenderProperties props;
        props.position = collider->getPosition() - m_viewportPosition;
        props.size = collider->getSize();
        props.color = glm::vec4(1.0f, 0.0f, 0.0f, 0.3f); // Semi-transparent red
        renderer.drawRect(props.position, props.size, glm::vec3(props.color));
    }
#endif
}

void ObjectLayer::renderPortals() {
    auto& renderer = Renderer::getInstance();
    auto* portalTexture = ResourceManager::getInstance().getTexture("portal");

    if (!portalTexture) return;

    // First time initialization
    if (!m_portalSprite) {
        m_portalSprite = std::make_unique<SpriteSheet>(portalTexture, 32, 32);
        m_portalAnimation = std::make_unique<AnimationController>();

        // Create portal animation
        auto portalAnim = std::make_unique<Animation>("portal");
        for (int i = 0; i < 6; ++i) {
            portalAnim->addFrame(i, 0.1f);  // 0.1 seconds per frame
        }
        portalAnim->setLooping(true);
        m_portalAnimation->addAnimation("portal", std::move(portalAnim));
        m_portalAnimation->playAnimation("portal");
    }

    // Update animation
    if (m_portalAnimation) {
        m_portalAnimation->update(1.0f / 60.0f);  // Assume 60fps
    }

    // Render each portal
    for (const auto& portal : m_portals) {
        RenderProperties props;
        props.position = portal.position - m_viewportPosition;
        props.size = portal.size;

        if (m_portalAnimation) {
            props.region = m_portalAnimation->getCurrentRegion(*m_portalSprite);
        }

        renderer.drawTexturedQuad(portalTexture, props);
    }
}

void ObjectLayer::clearObjects() {
    m_colliders.clear();
    m_portals.clear();
}