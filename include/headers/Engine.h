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
    bool m_spriteFlipX = false;

    BoxCollider* getPlayerCollider();
    GLFWwindow* getWindow() { return m_window; }

    void setCameraOffset(const glm::vec2& offset) { m_cameraOffset = offset; }
    const glm::vec2& getCameraOffset() const { return m_cameraOffset; }

private:
    enum class CharacterState {
        Idle,
        Walking,
        Attacking,
        Jumping
        // other state
    };

    struct CharacterSprites {
        std::unique_ptr<SpriteSheet> walkSheet;
        std::unique_ptr<SpriteSheet> idleSheet;
    };

    Engine() : m_window(nullptr), m_isRunning(false) {}
    ~Engine() { shutdown(); }
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    void processInput();
    void update(float deltaTime);
    void render();
    void handleMovement(float deltaTime);
    void handleCollisions();

    void initializeCameraOffset();

    GLFWwindow* m_window;
    bool m_isRunning;
    float m_lastFrame;

    glm::vec2 m_playerPosition;
    float m_moveSpeed;

    glm::vec2 getSpriteRenderPosition() const;
    glm::vec2 getColliderOffset() const;

    static constexpr glm::vec2 SPRITE_SIZE{ 100.0f, 64.0f };
    static constexpr glm::vec2 COLLIDER_SIZE{ 40.0f, 50.0f };

    std::unique_ptr<AnimationController> m_animationController;
    std::unordered_map<CharacterState, std::unique_ptr<SpriteSheet>> m_stateSprites;
    CharacterState m_currentState;
    CharacterSprites m_characterSprites;

    bool m_isWalking;

    std::vector<BoxCollider*> m_colliders;  // collision body manage
    BoxCollider* m_playerCollider;          // player collision body

    glm::vec2 m_cameraOffset{ 0.0f, 0.0f };

};