#pragma once
#include <glm/glm.hpp>

enum class ColliderType {
    Box,
    Circle
};


struct CollisionLayer {
    uint32_t layer = 0x0001;    
    uint32_t mask = 0xFFFFFFFF; 
};

// collision result info
struct CollisionManifold {
    bool hasCollision = false;       // if collision 
    glm::vec2 normal{ 0.0f, 0.0f };   // collison normal
    float penetration = 0.0f;        // Penetration Depth

    CollisionManifold() = default;
    explicit CollisionManifold(bool collision) : hasCollision(collision) {}
};