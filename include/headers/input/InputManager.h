#pragma once
#include "InputTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <queue>
#include <GLFW/glfw3.h>

class InputManager {
public:
    static InputManager& getInstance() {
        static InputManager instance;
        return instance;
    }

    void initialize(GLFWwindow* window);
    void update();
    void shutdown();

    // Event registration
    void registerCallback(InputEventType type, InputCallback callback);
    void removeCallback(InputEventType type, size_t callbackId);

    // Input state queries
    bool isKeyPressed(int keyCode) const;
    bool isButtonPressed(int deviceID, int buttonCode) const;
    glm::vec2 getAxisValue(int deviceID, int axisCode) const;

    // Event queue management
    void pushEvent(const InputEvent& event);
    void processEvents();

private:
    InputManager() = default;
    ~InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    // GLFW callbacks
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void joystickCallback(int jid, int event);

    struct CallbackData {
        InputCallback callback;
        size_t id;
    };

    GLFWwindow* m_window;
    std::unordered_map<InputEventType, std::vector<CallbackData>> m_callbacks;
    std::queue<InputEvent> m_eventQueue;
    size_t m_nextCallbackId = 0;

    // Device states
    std::unordered_map<int, bool> m_keyStates;
    std::unordered_map<int, std::unordered_map<int, bool>> m_buttonStates;
    std::unordered_map<int, std::unordered_map<int, glm::vec2>> m_axisStates;
};