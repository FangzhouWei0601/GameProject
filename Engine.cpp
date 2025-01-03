#include "Engine.h"
#include <iostream>
#include "renderer.h"
#include "ResourceManager.h"
#include "SpriteSheet.h"

bool Engine::initialize(const std::string& windowTitle, int width, int height) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    m_window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // Initialize input systems
    InputManager::getInstance().initialize(m_window);
    InputMapper::getInstance().setupDefaultBindings();

    // Initialize player position and speed
    m_playerPosition = glm::vec2(width / 2.0f, height / 2.0f);
    m_moveSpeed = 200.0f; // pixels per second

    m_isRunning = true;
    m_lastFrame = 0.0f;

    auto& resourceManager = ResourceManager::getInstance();
    if (!resourceManager.loadTexture("test", "E:/tmp/texture/Grass_01-512x512.png")) {
        std::cerr << "Failed to load test texture" << std::endl;
    }

    TextureData* spriteSheet = ResourceManager::getInstance().getTexture("test");
    if (spriteSheet) {
        m_characterSprites = std::make_unique<SpriteSheet>(spriteSheet, 32, 32);
        m_animationController = std::make_unique<AnimationController>();

        // Create idle animation
        auto idleAnim = std::make_unique<Animation>("idle");
        idleAnim->addFrame(0, 0.2f);
        idleAnim->addFrame(1, 0.2f);
        idleAnim->addFrame(2, 0.2f);
        idleAnim->addFrame(1, 0.2f);

        // Create walk animation
        auto walkAnim = std::make_unique<Animation>("walk");
        walkAnim->addFrame(3, 0.1f);
        walkAnim->addFrame(4, 0.1f);
        walkAnim->addFrame(5, 0.1f);
        walkAnim->addFrame(4, 0.1f);

        m_animationController->addAnimation("idle", std::move(idleAnim));
        m_animationController->addAnimation("walk", std::move(walkAnim));
        m_animationController->playAnimation("idle");
    }

    Renderer::getInstance().initialize(width, height);


    return true;
}

void Engine::handleMovement(float deltaTime) {
    glm::vec2 moveDirection(0.0f);

    // Check each direction
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_UP))
        moveDirection.y -= 1.0f;
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_DOWN))
        moveDirection.y += 1.0f;
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_LEFT))
        moveDirection.x -= 1.0f;
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_RIGHT))
        moveDirection.x += 1.0f;

    // Normalize diagonal movement
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
        m_playerPosition += moveDirection * m_moveSpeed * deltaTime;
    }
}

void Engine::processInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        m_isRunning = false;
    }
}

void Engine::update(float deltaTime) {
    // Update input manager
    InputManager::getInstance().update();

    // Handle movement
    handleMovement(deltaTime);

    m_frameTime += deltaTime;
    if (m_frameTime >= m_frameDuration) {
        m_frameTime -= m_frameDuration;
        m_currentFrame = (m_currentFrame + 1) % m_characterSprites->getFrameCount();
    }

    // Update animation
    if (m_animationController) {
        m_animationController->update(deltaTime);

        // Switch between idle and walk animations based on movement
        glm::vec2 moveDirection(0.0f);
        if (InputManager::getInstance().isKeyPressed(GLFW_KEY_UP)) moveDirection.y -= 1.0f;
        if (InputManager::getInstance().isKeyPressed(GLFW_KEY_DOWN)) moveDirection.y += 1.0f;
        if (InputManager::getInstance().isKeyPressed(GLFW_KEY_LEFT)) moveDirection.x -= 1.0f;
        if (InputManager::getInstance().isKeyPressed(GLFW_KEY_RIGHT)) moveDirection.x += 1.0f;

        if (glm::length(moveDirection) > 0.0f) {
            m_animationController->playAnimation("walk");
        }
        else {
            m_animationController->playAnimation("idle");
        }
    }
}


void Engine::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_characterSprites && m_animationController) {
        RenderProperties props;
        props.position = m_playerPosition;
        props.size = glm::vec2(64.0f, 64.0f);
        props.region = m_animationController->getCurrentRegion(*m_characterSprites);

        Renderer::getInstance().drawTexturedQuad(m_characterSprites->getTexture(), props);
    }

    glfwSwapBuffers(m_window);
}

void Engine::run() {
    while (m_isRunning && !glfwWindowShouldClose(m_window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        processInput();
        update(deltaTime);
        render();

        glfwPollEvents();
    }
}

void Engine::shutdown() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
    InputManager::getInstance().shutdown();
    Renderer::getInstance().shutdown();
}
