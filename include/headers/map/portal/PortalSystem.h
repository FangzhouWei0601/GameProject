#pragma once
#include "../../collision/BoxCollider.h"
#include "../MapTypes.h"
#include <vector>

class PortalSystem {
public:
    static bool checkPortalTrigger(const BoxCollider* playerCollider,
        const std::vector<PortalData>& portals,
        PortalData& outPortal);

private:
    static bool isInPortalRange(const BoxCollider* playerCollider,
        const PortalData& portal);
    static bool checkPortalConditions(const PortalData& portal);
};