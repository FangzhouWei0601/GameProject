#include <algorithm>
#include "../../../include/headers/collision/BoxCollider.h"

BoxCollider::BoxCollider(const glm::vec2& pos, const glm::vec2& size)
    : m_position(pos), m_size(size) {
}

bool BoxCollider::isColliding(const ICollider* other) const {
    // first check collision level
    if (!(m_layer.mask & other->getCollisionLayer().layer)) {
        return false;
    }

    if (const BoxCollider* boxOther = dynamic_cast<const BoxCollider*>(other)) {
        glm::vec2 minA = getMin();
        glm::vec2 maxA = getMax();
        glm::vec2 minB = boxOther->getMin();
        glm::vec2 maxB = boxOther->getMax();

        return (minA.x <= maxB.x && maxA.x >= minB.x &&
            minA.y <= maxB.y && maxA.y >= minB.y);
    }
    return false;
}

CollisionManifold BoxCollider::checkCollision(const ICollider* other) const {
    if (!(m_layer.mask & other->getCollisionLayer().layer)) {
        return CollisionManifold(false);
    }

    if (const BoxCollider* boxOther = dynamic_cast<const BoxCollider*>(other)) {
        glm::vec2 minA = getMin();
        glm::vec2 maxA = getMax();
        glm::vec2 minB = boxOther->getMin();
        glm::vec2 maxB = boxOther->getMax();

        glm::vec2 overlap(
            std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x),
            std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y)
        );

        if (overlap.x > 0 && overlap.y > 0) {
            CollisionManifold manifold(true);

            // Select the axis of minimum overlap as the separating axis
            if (overlap.x < overlap.y) {
                manifold.normal.x = (minA.x + maxA.x < minB.x + maxB.x) ? 1.0f : -1.0f;
                manifold.penetration = overlap.x;
            }
            else {
                manifold.normal.y = (minA.y + maxA.y < minB.y + maxB.y) ? 1.0f : -1.0f;
                manifold.penetration = overlap.y;
            }
            return manifold;
        }
    }
    return CollisionManifold(false);
}