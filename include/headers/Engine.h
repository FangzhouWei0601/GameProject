#pragma once
#include <memory>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../../include/headers/input/InputManager.h"
#include "../../include/headers/input/InputMapper.h"
#include <glm/glm.hpp>
#include "../../include/headers/renderer/SpriteSheet.h"
#include "../../include/headers/renderer/Animation.h"
#include "../../include/headers/collision/BoxCollider.h"
#include <vector>

class Engine {
public:
    static Engine& getInstance() {
        static Engine instance;
        return instance;
    }

    bool initialize(const std::string& windowTitle, int width, int height);
    void run();
    void shutdown();
    bool isRunning() const { return m_isRunning; }
    GLFWwindow* getWindow() { return m_window; }

private:
    Engine() : m_window(nullptr), m_isRunning(false) {}
    ~Engine() { shutdown(); }
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    void processInput();
    void update(float deltaTime);
    void render();
    void handleMovement(float deltaTime);
    void handleCollisions();

    GLFWwindow* m_window;
    bool m_isRunning;
    float m_lastFrame;

    glm::vec2 m_playerPosition;
    float m_moveSpeed;

    // Sprite management
    std::unique_ptr<SpriteSheet> m_characterSprites;
    std::unique_ptr<AnimationController> m_animationController;
    int m_currentFrame;
    float m_frameTime;     // Time accumulator for animation
    float m_frameDuration; // Duration of each frame

    std::vector<BoxCollider*> m_colliders;  // collision body manage
    BoxCollider* m_playerCollider;          // player collision body

};