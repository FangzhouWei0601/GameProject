#pragma once
#include <functional>
#include <string>
#include <glm/glm.hpp>

enum class InputEventType {
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_HELD,
    BUTTON_PRESSED,
    BUTTON_RELEASED,
    BUTTON_HELD,
    AXIS_MOVED,
    DEVICE_CHANGED
};

struct InputEvent {
    InputEventType type;
    int deviceID;      // Keyboard = 0, Gamepad IDs start from 1
    int keyCode;       // GLFW key code or gamepad button
    glm::vec2 axisValue;  // For analog inputs
    float timestamp;

    InputEvent(InputEventType t, int device, int key = -1,
        glm::vec2 axis = glm::vec2(0.0f), float time = 0.0f)
        : type(t), deviceID(device), keyCode(key), axisValue(axis), timestamp(time) {
    }
};

enum class GameAction {
    MOVE,
    JUMP,
    ATTACK,
    LAUNCH_ATTACK,
    CANCEL,
    DODGE,
    TRANSCEND_1,
    TRANSCEND_2,
    AWAKENING,
    SKILL_1,
    SKILL_2,
    SKILL_3,
    SKILL_4,
    SKILL_5,
    SKILL_6,
    // Add more as needed
};

struct InputBinding {
    int primaryKey;        // Keyboard key or gamepad button
    int modifierKey;       // For combinations (-1 if none)
    int deviceID;          // 0 for keyboard, 1+ for gamepads
    bool isAxisBinding;    // True for analog inputs
    int axisID;           // For analog inputs (-1 if none)

    InputBinding(int primary = -1, int modifier = -1, int device = 0,
        bool isAxis = false, int axis = -1)
        : primaryKey(primary), modifierKey(modifier), deviceID(device),
        isAxisBinding(isAxis), axisID(axis) {
    }
};

using InputCallback = std::function<void(const InputEvent&)>;