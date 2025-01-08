#include "../../../include/headers/map/ObjectLayer.h"
#include "../../../include/headers/renderer/Renderer.h"
#include "../../../include/headers/resource/ResourceManager.h"

void ObjectLayer::update(float deltaTime) {
    // 
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

    // 仅在调试模式下渲染碰撞体
#ifdef _DEBUG
    for (const auto& collider : m_colliders) {
        RenderProperties props;
        props.position = collider->getPosition() - m_viewportPosition;
        props.size = collider->getSize();
        props.color = glm::vec4(1.0f, 0.0f, 0.0f, 0.3f); // 半透明红色
        renderer.drawRect(props.position, props.size, glm::vec3(props.color));
    }
#endif
}

void ObjectLayer::renderPortals() {
    auto& renderer = Renderer::getInstance();
    auto* portalTexture = ResourceManager::getInstance().getTexture("portal");

    if (!portalTexture) return;

    // 首次初始化
    if (!m_portalSprite) {
        m_portalSprite = std::make_unique<SpriteSheet>(portalTexture, 32, 32);
        m_portalAnimation = std::make_unique<AnimationController>();

        // 创建portal动画
        auto portalAnim = std::make_unique<Animation>("portal");
        for (int i = 0; i < 6; ++i) {
            portalAnim->addFrame(i, 0.5f);  // 每帧0.1秒
        }
        portalAnim->setLooping(true);
        m_portalAnimation->addAnimation("portal", std::move(portalAnim));
        m_portalAnimation->playAnimation("portal");
    }

    // 更新动画
    if (m_portalAnimation) {
        m_portalAnimation->update(1.0f / 60.0f);  // 假设60fps
    }

    // 渲染每个传送门
    for (const auto& portal : m_portals) {
        RenderProperties props;
        props.position = portal.position - m_viewportPosition;
        props.size = portal.size;

        if (m_portalAnimation) {
            // 获取当前动画帧的UV坐标
            props.region = m_portalAnimation->getCurrentRegion(*m_portalSprite);
        }

        renderer.drawTexturedQuad(portalTexture, props);
    }
}

void ObjectLayer::clearObjects() {
    m_colliders.clear();
    m_portals.clear();
}