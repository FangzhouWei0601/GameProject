#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <functional>

enum class AreaType {
    Normal,         
    Safe,          
    Battle,        
    Boss,          
    Secret         
};

struct AreaBounds {
    glm::vec2 position;  
    glm::vec2 size;      

    bool containsPoint(const glm::vec2& point) const {
        return point.x >= position.x && point.x < position.x + size.x &&
            point.y >= position.y && point.y < position.y + size.y;
    }
};

struct AreaData {
    std::string id;           
    std::string name;         
    AreaType type;           
    AreaBounds bounds;       
    bool isDiscovered;       
    bool isUnlocked;        

    AreaData() : type(AreaType::Normal), isDiscovered(false), isUnlocked(true) {}

    glm::vec2 getCameraBoundsMin() const {
        return bounds.position;
    }

    glm::vec2 getCameraBoundsMax() const {
        return bounds.position + bounds.size;
    }
};

enum class PortalConditionType {
    None,           // No condition
    Key,            // Requires specific key
    Quest,          // Requires quest completion
    Level,          // Requires specific level
    Custom          // Custom condition
};

// Add new condition struct
struct PortalCondition {
    PortalConditionType type = PortalConditionType::None;

    // Condition parameters
    std::string requiredId;     // Key/Quest ID
    int requiredLevel = 0;      // Required level
    bool isMet = false;         // Condition state

    // Custom condition callback
    std::function<bool()> customCheck = nullptr;
};

struct PortalData {
    std::string targetAreaId;   // Target area identifier
    glm::vec2 position;         // Portal position
    glm::vec2 size;            // Portal size
    glm::vec2 targetPosition;   // Spawn position in target area
    bool isLocked;             // Lock state
    PortalCondition condition;  // Portal condition

    PortalData() : isLocked(false) {}
};