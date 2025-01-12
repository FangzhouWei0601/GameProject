#include "../../../include/headers/Engine.h"
#include <iostream>
#include "../../../include/headers/renderer/Renderer.h"
#include "../../../include/headers/resource/ResourceManager.h"
#include "../../../include/headers/renderer/SpriteSheet.h"
#include "../../../include/headers/map/MapManager.h"
#include "../../../include/headers/renderer/Animation.h"
#include "../../../include/headers/input/InputManager.h"
#include "../../../include/headers/input/InputMapper.h"
#include "../../../include/headers/collision/CollisionTypes.h"
#include "../../../include/headers/collision/BoxCollider.h"
#include "../../../include/headers/CommonDefines.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Renderer::getInstance().onWindowResize(width, height);
}

bool Engine::initialize(const std::string& windowTitle, int width, int height) {
    // Initialize GLFW
    if (!glfwInit()) {
        DEBUG_LOG_ERROR("Failed to initialize GLFW");
        return false;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    m_window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
    if (!m_window) {
        DEBUG_LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        DEBUG_LOG_ERROR("Failed to initialize GLAD");
        return false;
    }

    // Initialize input systems
    InputManager::getInstance().initialize(m_window);
    InputMapper::getInstance().setupDefaultBindings();

    // Initialize player position and speed
    m_playerPosition = glm::vec2(100.0f, 300.0f); 
    m_moveSpeed = 250.0f;


    m_isRunning = true;
    m_lastFrame = 0.0f;


    const glm::vec2 SPRITE_SIZE(100.0f, 64.0f);  
    const float COLLIDER_SCALE = 0.7f;          
    glm::vec2 colliderSize = SPRITE_SIZE * COLLIDER_SCALE;


    m_playerCollider = new BoxCollider(
        m_playerPosition + getColliderOffset(),  
        COLLIDER_SIZE                           
    );

    if (m_playerCollider) {
        m_playerCollider->setCollisionLayer(static_cast<uint32_t>(CollisionLayerBits::Player)); // 0x0001
        m_playerCollider->setCollisionMask(
            static_cast<uint32_t>(CollisionLayerBits::Door) |    // 0x0004
            static_cast<uint32_t>(CollisionLayerBits::Trigger)   // 0x0002
        );
        //DEBUG_LOG("Player collider initialized - Size: (" << colliderSize.x << ", " << colliderSize.y << ")");
    }

    auto& resourceManager = ResourceManager::getInstance();
    resourceManager.createResourceDirectories();
    resourceManager.initialize();

    const std::vector<std::pair<std::string, std::string>> texturesToLoad = {
        {"portal", "resources/textures/portal/portal.png"},
        {"test_area_1_bg", "resources/textures/backgrounds/test_area_1_bg.png"},
        {"test_area_2_bg", "resources/textures/backgrounds/test_area_2_bg.png"},
        {"characterwalk","resources/textures/characters/Walking_KG_1.png"},
        {"characteridle","resources/textures/characters/Idle_KG_1.png"}
    };

    const std::vector<std::pair<std::string, std::string>> soundsToLoad = {
    {"test_area_1_bgm", "resources/audio/bgm/background.wav"},
    {"test_area_2_bgm", "resources/audio/bgm/background2.wav"},
    {"trigger", "resources/audio/sfx/trigger.wav"},
    {"door", "resources/audio/sfx/door.wav"}
    };

    for (const auto& [name, path] : texturesToLoad) {
        if (!std::filesystem::exists(path)) {
            DEBUG_LOG_WARN("Texture file not found: " << path);
            continue;
        }
        if (!resourceManager.loadTexture(name, path)) {
            DEBUG_LOG_ERROR("Failed to load texture: " << name);
        }
    }

    for (const auto& [name, path] : soundsToLoad) {
        if (!std::filesystem::exists(path)) {
            DEBUG_LOG_WARN("Sound file not found: " << path);
            continue;
        }
        if (!resourceManager.loadSound(name, path)) {
            DEBUG_LOG_ERROR("Failed to load sound: " << name);
        }
    }

    m_animationController = std::make_unique<AnimationController>();
    m_isWalking = false;
    m_currentState = CharacterState::Idle;

    TextureData* walkTexture = resourceManager.getTexture("characterwalk");
    if (walkTexture) {
        m_characterSprites.walkSheet = std::make_unique<SpriteSheet>(walkTexture, 100, 64);

        auto walkAnim = std::make_unique<Animation>("walk");
        for (int i = 0; i < 7; ++i) {
            walkAnim->addFrame(i, 0.1f);
        }
        walkAnim->setLooping(true);
        m_animationController->addAnimation("walk", std::move(walkAnim));
    }

    // 加载站立动画
    TextureData* idleTexture = resourceManager.getTexture("characteridle");
    if (idleTexture) {
        m_characterSprites.idleSheet = std::make_unique<SpriteSheet>(idleTexture, 100, 64);

        auto idleAnim = std::make_unique<Animation>("idle");
        for (int i = 0; i < 4; ++i) {
            idleAnim->addFrame(i, 0.3f);
        }
        idleAnim->setLooping(true);
        m_animationController->addAnimation("idle", std::move(idleAnim));
    }

    // 设置初始动画
    if (m_animationController) {
        m_animationController->playAnimation("idle");
    }

    Renderer::getInstance().initialize(width, height);

    auto* camera = Renderer::getInstance().getCamera();
    camera->setFollowSpeed(4.0f);
    camera->setZoom(1.0f);

    // load and switch to test aera
    auto& mapManager = MapManager::getInstance();
    if (!mapManager.loadArea("test_area_1", ResourceManager::getMapPath("test_area_1"))) {
        std::cerr << "Failed to load test_area_1" << std::endl;
        return false;
    }
    if (!mapManager.loadArea("test_area_2", ResourceManager::getMapPath("test_area_2"))) {
        std::cerr << "Failed to load test_area_2" << std::endl;
        return false;
    }

    mapManager.changeArea("test_area_1", glm::vec2(100.0f, 300.0f));

    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    return true;
}

void Engine::handleMovement(float deltaTime) {
    glm::vec2 moveDirection(0.0f);
    bool directionChanged = false;

    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_UP))
        moveDirection.y -= 1.0f;
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_DOWN))
        moveDirection.y += 1.0f;
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_LEFT)) {
        moveDirection.x -= 1.0f;
        bool wasFlipped = m_spriteFlipX;
        m_spriteFlipX = true;
        directionChanged = (wasFlipped != m_spriteFlipX);
    }
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_RIGHT)) {
        moveDirection.x += 1.0f;
        bool wasFlipped = m_spriteFlipX;
        m_spriteFlipX = false;
        directionChanged = (wasFlipped != m_spriteFlipX);
    }

    if (glm::length(moveDirection) > 0.0f || directionChanged) {
        if (glm::length(moveDirection) > 0.0f) {
            moveDirection = glm::normalize(moveDirection);
            glm::vec2 nextPosition = m_playerPosition + moveDirection * m_moveSpeed * deltaTime;
            glm::vec2 nextColliderPos = nextPosition + getColliderOffset();

            // 保存原始位置
            glm::vec2 originalPosition = m_playerPosition;
            glm::vec2 originalColliderPos = m_playerCollider->getPosition();

            // 尝试移动
            m_playerCollider->setPosition(nextColliderPos);

            bool canMove = true;
            if (auto* currentArea = MapManager::getInstance().getCurrentArea()) {
                for (const auto& [id, mechanism] : currentArea->getMechanisms()) {
                    if (auto* mechCollider = mechanism->getCollider()) {
                        if (m_playerCollider->isColliding(mechCollider)) {
                            canMove = false;
                            break;
                        }
                    }
                }
            }

            if (canMove) {
                m_playerPosition = nextPosition;
            }
            else {
                m_playerPosition = originalPosition;
                m_playerCollider->setPosition(originalColliderPos);
            }
        }

        // 无论是否移动成功，都要更新碰撞箱位置
        glm::vec2 finalColliderPos = m_playerPosition + getColliderOffset();
        m_playerCollider->setPosition(finalColliderPos);
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

    // Update animation
    if (m_animationController) {
        glm::vec2 moveDirection(0.0f);
        if (InputManager::getInstance().isKeyPressed(GLFW_KEY_UP)) moveDirection.y -= 1.0f;
        if (InputManager::getInstance().isKeyPressed(GLFW_KEY_DOWN)) moveDirection.y += 1.0f;
        if (InputManager::getInstance().isKeyPressed(GLFW_KEY_LEFT)) moveDirection.x -= 1.0f;
        if (InputManager::getInstance().isKeyPressed(GLFW_KEY_RIGHT)) moveDirection.x += 1.0f;

        bool isMoving = glm::length(moveDirection) > 0.0f;
        if (isMoving != m_isWalking) {
            m_isWalking = isMoving;
            m_animationController->playAnimation(m_isWalking ? "walk" : "idle");
        }

        m_animationController->update(deltaTime);
    }

    // Update area mechanisms
    if (auto* currentArea = MapManager::getInstance().getCurrentArea()) {
        currentArea->updateMechanisms(deltaTime);
    }

    MapManager::getInstance().update(deltaTime);

    // Update camera
    glm::vec2 targetPos = m_playerPosition + m_cameraOffset;
    Renderer::getInstance().getCamera()->setTarget(targetPos);
    Renderer::getInstance().getCamera()->updateFollow(deltaTime);

    Renderer::getInstance().updateCamera(deltaTime);
    // Handle collisions
    handleCollisions();

    // Handle portal transitions
    auto& mapManager = MapManager::getInstance();
    if (mapManager.handlePortalTransition(m_playerCollider)) {
        std::cout << "Portal transition triggered" << std::endl;
    }
}

void Engine::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    MapManager::getInstance().render();

    if (m_animationController) {
        RenderProperties props;
        props.position = m_playerPosition;
        // Width compensation
        if (m_spriteFlipX) {
            props.position.x += SPRITE_SIZE.x;
        }
        props.size = SPRITE_SIZE;
        props.flipX = m_spriteFlipX;

        SpriteSheet* currentSheet = m_isWalking ?
            m_characterSprites.walkSheet.get() :
            m_characterSprites.idleSheet.get();

        if (currentSheet) {
            props.region = m_animationController->getCurrentRegion(*currentSheet);
            Renderer::getInstance().drawTexturedQuad(currentSheet->getTexture(), props);
        }
    }

    if (m_playerCollider) {
        Renderer::getInstance().drawRect(
            m_playerCollider->getPosition(),
            m_playerCollider->getSize(),
            glm::vec3(0.0f, 1.0f, 0.0f),
            0.3f
        );
    }

    if (auto* currentArea = MapManager::getInstance().getCurrentArea()) {
        for (const auto& [id, mechanism] : currentArea->getMechanisms()) {
            if (auto* collider = mechanism->getCollider()) {
                glm::vec3 color;
                if (id.find("door") != std::string::npos) {
                    color = glm::vec3(1.0f, 0.0f, 0.0f);  // door
                }
                else if (id.find("trigger") != std::string::npos) {
                    color = glm::vec3(1.0f, 1.0f, 0.0f);  // trigger
                }

                Renderer::getInstance().drawRect(
                    collider->getPosition(),
                    collider->getSize(),
                    color,
                    0.3f  // transparency
                );
            }
        }
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
    delete m_playerCollider;
    for (auto* collider : m_colliders) {
        delete collider;
    }
    m_colliders.clear();
}

void Engine::handleCollisions() {
    if (!m_playerCollider) {
        DEBUG_LOG_ERROR("Player collider is null!");
        return;
    }

    const glm::vec2 SPRITE_SIZE(100.0f, 64.0f);
    const float COLLIDER_SCALE = 0.7f;
    glm::vec2 colliderSize = SPRITE_SIZE * COLLIDER_SCALE;
    glm::vec2 colliderOffset = (SPRITE_SIZE - colliderSize) * 0.5f;

    if (auto* currentArea = MapManager::getInstance().getCurrentArea()) {
        for (const auto& [id, mechanism] : currentArea->getMechanisms()) {
            if (auto* mechCollider = mechanism->getCollider()) {
                CollisionManifold manifold = m_playerCollider->checkCollision(mechCollider);
                if (manifold.hasCollision) {
                    //DEBUG_LOG("Collision detected with mechanism: " << id);
                    m_playerPosition += manifold.normal * manifold.penetration;
                    m_playerCollider->setPosition(m_playerPosition + colliderOffset);
                }
            }
        }
    }

    for (const auto* collider : m_colliders) {
        if (collider == m_playerCollider) continue;

        CollisionManifold manifold = m_playerCollider->checkCollision(collider);
        if (manifold.hasCollision) {
            m_playerPosition += manifold.normal * manifold.penetration;
            m_playerCollider->setPosition(m_playerPosition + colliderOffset);
        }
    }
}

BoxCollider* Engine::getPlayerCollider() {
    if (m_playerCollider) {
        //DEBUG_LOG("Getting player collider at:(" << m_playerPosition.x << "," << m_playerPosition.y << "(");
    }
    else {
        std::cout << "Warning: Player collider is null!" << std::endl;
    }
    return m_playerCollider;
}

glm::vec2 Engine::getColliderOffset() const {
    return glm::vec2(
        (SPRITE_SIZE.x - COLLIDER_SIZE.x) * 0.5f,  // 水平居中
        (SPRITE_SIZE.y - COLLIDER_SIZE.y) * 0.8f   // 底部稍微上移，让碰撞箱更贴近角色
    );
}

glm::vec2 Engine::getSpriteRenderPosition() const {
    return m_playerPosition;
}

void Engine::initializeCameraOffset() {
    const auto& screenSize = Renderer::getInstance().getScreenSize();
    m_cameraOffset = glm::vec2(screenSize.x * 0.5f, screenSize.y * 0.5f);
}

