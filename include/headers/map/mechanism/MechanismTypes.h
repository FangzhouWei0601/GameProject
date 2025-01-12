#pragma once
#include <functional>
#include <string>
#include <glm/glm.hpp>

enum class MechanismType {
    Trigger,          // Trigger mechanism
    Sequence,         // Sequence mechanism
    Breakable,        // Breakable mechanism
    MovingPlatform,   // Moving platform
    Trap,             // Trap mechanism
    Door,             // Door mechanism
    BuffZone          // Buff zone mechanism
};

enum class MechanismState {
    Inactive,    // Mechanism is inactive
    Active,      // Mechanism is active
    Finished,    // Mechanism has finished its action
    Broken       // Mechanism is broken
};

struct TriggerCondition {
    bool requiresPlayerPresence = false;  // Requires player to be present
    bool requiresInteraction = false;     // Requires player interaction
    float triggerRadius = 0.0f;          // Activation radius

    // Custom condition callback
    std::function<bool()> customCondition = nullptr;
};

enum class EffectType {
    None,
    Damage,         // Deals damage
    Heal,           // Restores health
    Buff,           // Applies positive effect
    Debuff,         // Applies negative effect
    MovePlatform,   // Platform movement
    OpenGate,       // Opens a gate/door
    SpawnEnemy,     // Spawns enemy
    Teleport        // Teleports target
};

struct MechanismEffect {
    EffectType type = EffectType::None;
    std::string targetId;         // Target entity ID
    float duration = 0.0f;        // Effect duration
    float value = 0.0f;          // Effect value
    glm::vec2 direction{ 0.0f };  // Effect direction (if needed)

    // Effect timing parameters
    struct {
        bool isPeriodic = false;      // Is periodic effect
        float interval = 0.0f;        // Period interval
        float lastTriggerTime = 0.0f; // Last trigger time
    } timing;

    // Movement parameters
    struct {
        glm::vec2 startPos{ 0.0f };   // Start position
        glm::vec2 endPos{ 0.0f };     // End position
        float speed = 0.0f;           // Movement speed
        bool isLoop = false;          // Is looping movement
    } movement;
};