#pragma once
#include "CollisionTypes.h"

class ICollider {
public:
    virtual ~ICollider() = default;

    virtual void setPosition(const glm::vec2& pos) = 0;
    virtual void setSize(const glm::vec2& size) = 0;

    virtual glm::vec2 getPosition() const = 0;
    virtual glm::vec2 getSize() const = 0;

    virtual bool isColliding(const ICollider* other) const = 0;
    virtual CollisionManifold checkCollision(const ICollider* other) const = 0;

    // Åö×²¹ýÂËÏà¹Ø
    void setCollisionLayer(uint32_t layer) { m_layer.layer = layer; }
    void setCollisionMask(uint32_t mask) { m_layer.mask = mask; }
    const CollisionLayer& getCollisionLayer() const { return m_layer; }

protected:
    CollisionLayer m_layer;
};