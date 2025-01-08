#include <algorithm>
#include "../../../include/headers/collision/BoxCollider.h"
#include "../../../include/headers/CommonDefines.h"


BoxCollider::BoxCollider(const glm::vec2& pos, const glm::vec2& size) {
    if (!isValidPosition(pos)) {
        DEBUG_LOG_WARN("Invalid position in BoxCollider. Using default position.");
        m_position = glm::vec2(0.0f);
    }
    else {
        m_position = pos;
    }

    if (!isValidSize(size)) {
        DEBUG_LOG_WARN("Invalid size in BoxCollider. Using default size.");
        m_size = glm::vec2(MIN_SIZE);
    }
    else {
        m_size = size;
    }
}

bool BoxCollider::isValidPosition(const glm::vec2& pos) {
    return !std::isnan(pos.x) && !std::isnan(pos.y) &&
        std::abs(pos.x) <= MAX_POSITION && std::abs(pos.y) <= MAX_POSITION;
}

bool BoxCollider::isValidSize(const glm::vec2& size) {
    return !std::isnan(size.x) && !std::isnan(size.y) &&
        size.x >= MIN_SIZE && size.y >= MIN_SIZE &&
        size.x <= MAX_SIZE && size.y <= MAX_SIZE;
}

void BoxCollider::setPosition(const glm::vec2& pos) {
    if (isValidPosition(pos)) {
        m_position = pos;
    }
    else {
        DEBUG_LOG("Warning: Attempted to set invalid position in BoxCollider.");
    }
}

void BoxCollider::setSize(const glm::vec2& size) {
    if (isValidSize(size)) {
        m_size = size;
    }
    else {
        DEBUG_LOG("Warning: Attempted to set invalid size in BoxCollider.");
    }
}

bool BoxCollider::isInBounds() const {
    return isValidPosition(m_position) && isValidSize(m_size);
}

bool BoxCollider::checkOverlap(const BoxCollider* other) const {
    glm::vec2 minA = getMin();
    glm::vec2 maxA = getMax();
    glm::vec2 minB = other->getMin();
    glm::vec2 maxB = other->getMax();

    return (minA.x <= maxB.x && maxA.x >= minB.x &&
        minA.y <= maxB.y && maxA.y >= minB.y);
}

bool BoxCollider::isColliding(const ICollider* other) const {
    if (!other || !isInBounds()) {
        return false;
    }

    auto otherLayer = other->getCollisionLayer();
    DEBUG_LOG("Åö×²¼ì²éÑÚÂë:");
    DEBUG_LOG("- µ±Ç°²ã: 0x" << std::hex << m_layer.layer << " ÑÚÂë: 0x" << m_layer.mask);
    DEBUG_LOG("- ¶Ô·½²ã: 0x" << std::hex << otherLayer.layer << " ÑÚÂë: 0x" << otherLayer.mask);

    if (!(m_layer.mask & otherLayer.layer)) {
        DEBUG_LOG("ÑÚÂë¼ì²éÊ§°Ü");
        return false;
    }

    const BoxCollider* boxOther = dynamic_cast<const BoxCollider*>(other);
    if (!boxOther || !boxOther->isInBounds()) {
        return false;
    }

    bool result = checkOverlap(boxOther);
    if (result) {
        DEBUG_LOG("Åö×²³É¹¦: " << m_layer.layer);
    }
    return result;
}

CollisionManifold BoxCollider::checkCollision(const ICollider* other) const {
    if (!other || !isInBounds()) {
        return CollisionManifold(false);
    }

    if (!(m_layer.mask & other->getCollisionLayer().layer)) {
        return CollisionManifold(false);
    }

    const BoxCollider* boxOther = dynamic_cast<const BoxCollider*>(other);
    if (!boxOther || !boxOther->isInBounds()) {
        return CollisionManifold(false);
    }

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

    return CollisionManifold(false);
}