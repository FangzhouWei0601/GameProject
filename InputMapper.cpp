#include "InputMapper.h"
#include "InputManager.h"
#include <GLFW/glfw3.h>
#include <fstream>
//#include <json.hpp> // todo

void InputMapper::bindAction(GameAction action, const InputBinding& binding) {
    m_bindings[action].push_back(binding);
}

void InputMapper::unbindAction(GameAction action) {
    m_bindings.erase(action);
}

void InputMapper::clearAllBindings() {
    m_bindings.clear();
}

bool InputMapper::isActionTriggered(GameAction action) const {
    auto it = m_bindings.find(action);
    if (it == m_bindings.end()) return false;

    for (const auto& binding : it->second) {
        if (checkBinding(binding)) return true;
    }
    return false;
}

bool InputMapper::isActionHeld(GameAction action) const {
    auto it = m_bindings.find(action);
    if (it == m_bindings.end()) return false;

    for (const auto& binding : it->second) {
        if (checkCombination(binding)) return true;
    }
    return false;
}

glm::vec2 InputMapper::getActionAxis(GameAction action) const {
    auto it = m_bindings.find(action);
    if (it == m_bindings.end()) return glm::vec2(0.0f);

    for (const auto& binding : it->second) {
        if (binding.isAxisBinding) {
            return InputManager::getInstance().getAxisValue(binding.deviceID, binding.axisID);
        }
    }
    return glm::vec2(0.0f);
}

bool InputMapper::checkBinding(const InputBinding& binding) const {
    auto& inputManager = InputManager::getInstance();

    if (binding.isAxisBinding) {
        auto axisValue = inputManager.getAxisValue(binding.deviceID, binding.axisID);
        return glm::length(axisValue) > 0.1f;
    }

    if (binding.deviceID == 0) { // Keyboard
        if (binding.modifierKey != -1) {
            return inputManager.isKeyPressed(binding.primaryKey) &&
                inputManager.isKeyPressed(binding.modifierKey);
        }
        return inputManager.isKeyPressed(binding.primaryKey);
    }
    else { // Gamepad
        if (binding.modifierKey != -1) {
            return inputManager.isButtonPressed(binding.deviceID, binding.primaryKey) &&
                inputManager.isButtonPressed(binding.deviceID, binding.modifierKey);
        }
        return inputManager.isButtonPressed(binding.deviceID, binding.primaryKey);
    }
}

bool InputMapper::checkCombination(const InputBinding& binding) const {
    auto& inputManager = InputManager::getInstance();

    if (binding.deviceID == 0) { // Keyboard
        if (binding.modifierKey != -1) {
            return inputManager.isKeyPressed(binding.primaryKey) &&
                inputManager.isKeyPressed(binding.modifierKey);
        }
        return inputManager.isKeyPressed(binding.primaryKey);
    }
    else { // Gamepad
        if (binding.modifierKey != -1) {
            return inputManager.isButtonPressed(binding.deviceID, binding.primaryKey) &&
                inputManager.isButtonPressed(binding.deviceID, binding.modifierKey);
        }
        return inputManager.isButtonPressed(binding.deviceID, binding.primaryKey);
    }
}

void InputMapper::setupDefaultBindings() {
    clearAllBindings();

    // Movement (Keyboard: Arrow keys)
    bindAction(GameAction::MOVE, InputBinding(GLFW_KEY_UP));
    bindAction(GameAction::MOVE, InputBinding(GLFW_KEY_DOWN));
    bindAction(GameAction::MOVE, InputBinding(GLFW_KEY_LEFT));
    bindAction(GameAction::MOVE, InputBinding(GLFW_KEY_RIGHT));

    // Movement (Gamepad: Left stick)
    bindAction(GameAction::MOVE, InputBinding(-1, -1, 1, true, 0));  // deviceID=1, axisID=0

    // Basic Actions (Keyboard)
    bindAction(GameAction::ATTACK, InputBinding(GLFW_KEY_X));        // Normal Attack
    bindAction(GameAction::LAUNCH_ATTACK, InputBinding(GLFW_KEY_Z)); // Launch Attack
    bindAction(GameAction::JUMP, InputBinding(GLFW_KEY_C));          // Jump
    bindAction(GameAction::CANCEL, InputBinding(GLFW_KEY_SPACE));    // Cancel

    // Basic Actions (Gamepad - PS4/Xbox layout)
    bindAction(GameAction::ATTACK, InputBinding(1, -1, 1));          // Square/X
    bindAction(GameAction::LAUNCH_ATTACK, InputBinding(0, -1, 1));   // Triangle/Y
    bindAction(GameAction::JUMP, InputBinding(2, -1, 1));            // Circle/B
    bindAction(GameAction::CANCEL, InputBinding(3, -1, 1));          // Cross/A

    // Special Actions (Keyboard)
    bindAction(GameAction::TRANSCEND_1, InputBinding(GLFW_KEY_LEFT_SHIFT));
    bindAction(GameAction::TRANSCEND_2, InputBinding(GLFW_KEY_LEFT_ALT));
    bindAction(GameAction::AWAKENING, InputBinding(GLFW_KEY_LEFT_CONTROL));

    // Special Actions (Gamepad)
    bindAction(GameAction::TRANSCEND_1, InputBinding(6, -1, 1));     // L2/LT
    bindAction(GameAction::TRANSCEND_2, InputBinding(7, -1, 1));     // R2/RT
    bindAction(GameAction::AWAKENING, InputBinding(4, 5, 1));        // LB+RB

    // Skill Bindings (Keyboard)
    bindAction(GameAction::SKILL_1, InputBinding(GLFW_KEY_A));
    bindAction(GameAction::SKILL_2, InputBinding(GLFW_KEY_S));
    bindAction(GameAction::SKILL_3, InputBinding(GLFW_KEY_D));
    bindAction(GameAction::SKILL_4, InputBinding(GLFW_KEY_Q));
    bindAction(GameAction::SKILL_5, InputBinding(GLFW_KEY_W));
    bindAction(GameAction::SKILL_6, InputBinding(GLFW_KEY_E));
}