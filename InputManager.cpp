#include "InputManager.h"
#include <algorithm>
#include <iostream>

void InputManager::initialize(GLFWwindow* window) {
    m_window = window;

    // Set GLFW callbacks
    glfwSetKeyCallback(window, keyCallback);
    glfwSetJoystickCallback(joystickCallback);

    // Setup initial gamepad detection
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
        if (glfwJoystickPresent(jid)) {
            joystickCallback(jid, GLFW_CONNECTED);
        }
    }
}

void InputManager::update() {
    // Process queued events
    processEvents();

    // Update gamepad states
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
        if (glfwJoystickPresent(jid)) {
            int buttonCount;
            const unsigned char* buttons = glfwGetJoystickButtons(jid, &buttonCount);

            int axisCount;
            const float* axes = glfwGetJoystickAxes(jid, &axisCount);

            // Update button states and generate events
            for (int i = 0; i < buttonCount; ++i) {
                bool previousState = m_buttonStates[jid][i];
                bool currentState = buttons[i] == GLFW_PRESS;
                m_buttonStates[jid][i] = currentState;

                if (currentState && !previousState) {
                    pushEvent(InputEvent(InputEventType::BUTTON_PRESSED, jid, i));
                }
                else if (!currentState && previousState) {
                    pushEvent(InputEvent(InputEventType::BUTTON_RELEASED, jid, i));
                }
            }

            // Update axis states and generate events
            for (int i = 0; i < axisCount; i += 2) {
                if (i + 1 < axisCount) {
                    glm::vec2 axisValue(axes[i], axes[i + 1]);
                    if (glm::length(axisValue) > 0.1f) { // Dead zone
                        m_axisStates[jid][i / 2] = axisValue;
                        pushEvent(InputEvent(InputEventType::AXIS_MOVED, jid, i / 2, axisValue));
                    }
                }
            }
        }
    }
}

void InputManager::pushEvent(const InputEvent& event) {
    m_eventQueue.push(event);
}

void InputManager::processEvents() {
    while (!m_eventQueue.empty()) {
        const InputEvent& event = m_eventQueue.front();

        // Dispatch event to registered callbacks
        auto it = m_callbacks.find(event.type);
        if (it != m_callbacks.end()) {
            for (const auto& callbackData : it->second) {
                callbackData.callback(event);
            }
        }

        m_eventQueue.pop();
    }
}

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto& instance = getInstance();

    if (action == GLFW_PRESS) {
        instance.m_keyStates[key] = true;
        instance.pushEvent(InputEvent(InputEventType::KEY_PRESSED, 0, key));
    }
    else if (action == GLFW_RELEASE) {
        instance.m_keyStates[key] = false;
        instance.pushEvent(InputEvent(InputEventType::KEY_RELEASED, 0, key));
    }
}

void InputManager::joystickCallback(int jid, int event) {
    auto& instance = getInstance();

    if (event == GLFW_CONNECTED) {
        std::cout << "Gamepad " << jid << " connected" << std::endl;
        instance.pushEvent(InputEvent(InputEventType::DEVICE_CHANGED, jid));
    }
    else if (event == GLFW_DISCONNECTED) {
        std::cout << "Gamepad " << jid << " disconnected" << std::endl;
        instance.m_buttonStates.erase(jid);
        instance.m_axisStates.erase(jid);
        instance.pushEvent(InputEvent(InputEventType::DEVICE_CHANGED, jid));
    }
}

bool InputManager::isKeyPressed(int keyCode) const {
    auto it = m_keyStates.find(keyCode);
    return (it != m_keyStates.end() && it->second);
}

bool InputManager::isButtonPressed(int deviceID, int buttonCode) const {
    auto deviceIt = m_buttonStates.find(deviceID);
    if (deviceIt != m_buttonStates.end()) {
        auto buttonIt = deviceIt->second.find(buttonCode);
        return (buttonIt != deviceIt->second.end() && buttonIt->second);
    }
    return false;
}

glm::vec2 InputManager::getAxisValue(int deviceID, int axisCode) const {
    auto deviceIt = m_axisStates.find(deviceID);
    if (deviceIt != m_axisStates.end()) {
        auto axisIt = deviceIt->second.find(axisCode);
        if (axisIt != deviceIt->second.end()) {
            return axisIt->second;
        }
    }
    return glm::vec2(0.0f);
}

void InputManager::shutdown() {
    m_keyStates.clear();
    m_buttonStates.clear();
    m_axisStates.clear();
    m_callbacks.clear();

    while (!m_eventQueue.empty()) {
        m_eventQueue.pop();
    }
}