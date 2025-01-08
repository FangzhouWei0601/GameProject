#pragma once
#include "ILayer.h"
#include "../collision/BoxCollider.h"
#include "MapTypes.h"
#include <vector>
#include <memory>
#include "../renderer/SpriteSheet.h"
#include "../renderer/Animation.h"

class ObjectLayer : public ILayer {
public:
    ObjectLayer() = default;
    ~ObjectLayer() override = default;

    void update(float deltaTime) override;
    void render() override;

    // 对象管理
    void addCollider(std::unique_ptr<BoxCollider> collider);
    void addPortal(const PortalData& portal);
    void clearObjects();

private:
    std::vector<std::unique_ptr<BoxCollider>> m_colliders;
    std::vector<PortalData> m_portals;
    std::unique_ptr<SpriteSheet> m_portalSprite;
    std::unique_ptr<AnimationController> m_portalAnimation;

    void renderColliders();
    void renderPortals();
};