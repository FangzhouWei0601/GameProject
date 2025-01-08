#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "../resource/TextureData.h"
#include "../camera/Camera.h"

struct RenderProperties {
    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 size = glm::vec2(1.0f);
    float rotation = 0.0f;                  // Rotation in degrees
    glm::vec2 pivot = glm::vec2(0.5f);     // Pivot point (0,0 = top-left, 1,1 = bottom-right)
    bool flipX = false;
    bool flipY = false;
    glm::vec4 color = glm::vec4(1.0f);     // Tint color
    TextureRegion region;  // Added texture region
};

class Renderer {
public:
    static Renderer& getInstance() {
        static Renderer instance;
        return instance;
    }

    void initialize(int screenWidth, int screenHeight);
    void shutdown();

    // Add texture rendering methods
    void drawTexturedQuad(const glm::vec2& position, const glm::vec2& size, const TextureData* texture, const glm::vec4& color = glm::vec4(1.0f));
    void drawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, float alpha = 1.0f);

    void drawTexturedQuad(const TextureData* texture, const RenderProperties& props);

    void setScreenSize(const glm::vec2& size) { m_screenSize = size; }
    glm::vec2 getScreenSize() const { return m_screenSize; }

    Camera* getCamera() { return m_camera.get(); }
    void updateCamera(float deltaTime);

    void onWindowResize(int width, int height);

private:
    Renderer() = default;
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void initShaders();
    void initBuffers();

    GLuint m_shaderProgram;
    GLuint m_VBO, m_VAO, m_EBO;
    glm::mat4 m_projection;
    glm::mat4 calculateTransform(const RenderProperties& props);
    glm::vec2 m_screenSize{ 800.0f, 600.0f };

    std::unique_ptr<Camera> m_camera;

    GLuint m_textureShaderProgram;

    void initTextureShaders();
};