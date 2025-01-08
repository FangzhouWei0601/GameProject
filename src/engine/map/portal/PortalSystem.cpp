#include "../../../../include/headers/map/portal/PortalSystem.h"
#include <iostream>
#include "../../../../include/headers/CommonDefines.h"

bool PortalSystem::isInPortalRange(const BoxCollider* playerCollider,
    const PortalData& portal) {
    BoxCollider portalCollider(portal.position, portal.size);
    return playerCollider->isColliding(&portalCollider);
}

bool PortalSystem::checkPortalTrigger(const BoxCollider* playerCollider,
    const std::vector<PortalData>& portals,
    PortalData& outPortal)
{
    if (!playerCollider) return false;

    for (const auto& portal : portals) {
        if (portal.position.x < 0 || portal.position.y < 0 ||
            portal.size.x <= 0 || portal.size.y <= 0) {
            DEBUG_LOG_WARN("Invalid portal data detected");
            continue;
        }

        BoxCollider portalCollider(portal.position, portal.size);
        portalCollider.setCollisionLayer(0x0008);  // Portal layer
        portalCollider.setCollisionMask(0xFFFF);

        if (portalCollider.isColliding(playerCollider)) {
            DEBUG_LOG("Portal triggered at: (" << portal.position.x <<
                "," << portal.position.y << ")");
            outPortal = portal;
            return true;
        }
    }
    return false;
}

bool PortalSystem::checkPortalConditions(const PortalData& portal) {
    // If portal is locked, conditions cannot be met
    if (portal.isLocked) return false;

    // Check based on condition type
    switch (portal.condition.type) {
    case PortalConditionType::None:
        return true;

    case PortalConditionType::Key:
        return portal.condition.isMet;

    case PortalConditionType::Quest:
        return portal.condition.isMet;

    case PortalConditionType::Level:
        return portal.condition.isMet;

    case PortalConditionType::Custom:
        return portal.condition.customCheck ?
            portal.condition.customCheck() : false;

    default:
        return false;
    }
}