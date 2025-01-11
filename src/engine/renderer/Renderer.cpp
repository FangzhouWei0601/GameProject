#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "../../../include/headers/renderer/Renderer.h"
#include "../../../include/headers/resource/TextureData.h"
#include "../../../include/headers/input/InputManager.h"

const char* texturedVertexShaderSource = R"(
    #version 430 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    out vec2 TexCoord;
    
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

// Add textured fragment shader
const char* texturedFragmentShaderSource = R"(
    #version 430 core
    in vec2 TexCoord;
    
    uniform sampler2D textureImage;
    uniform vec4 color;
    
    out vec4 FragColor;
    
    void main() {
        FragColor = texture(textureImage, TexCoord) * color;
    }
)";

const char* vertexShaderSource = R"(
    #version 430 core
    layout (location = 0) in vec2 aPos;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
    }
)";

// Basic fragment shader for non-textured rendering
const char* fragmentShaderSource = R"(
    #version 430 core
    uniform vec3 color;
    uniform float alpha;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(color, alpha);
    }
)";

void Renderer::initialize(int screenWidth, int screenHeight) {
    initShaders();
    initTextureShaders();
    initBuffers();

    // Setup alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_projection = glm::ortho(0.0f, static_cast<float>(screenWidth),
        static_cast<float>(screenHeight), 0.0f, -1.0f, 1.0f);

    m_screenSize = glm::vec2(static_cast<float>(screenWidth),
        static_cast<float>(screenHeight));

    m_camera = std::make_unique<Camera>(screenWidth, screenHeight);
}

void Renderer::initTextureShaders() {
    // Create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &texturedVertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Create fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &texturedFragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create shader program
    m_textureShaderProgram = glCreateProgram();
    glAttachShader(m_textureShaderProgram, vertexShader);
    glAttachShader(m_textureShaderProgram, fragmentShader);
    glLinkProgram(m_textureShaderProgram);

    // Check for shader compilation and linking errors
    int success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
    }

    glGetProgramiv(m_textureShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_textureShaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Renderer::initBuffers() {
    float vertices[] = {
        // Positions    // Texture coords
        0.0f, 1.0f,    0.0f, 1.0f,  // top left
        1.0f, 1.0f,    1.0f, 1.0f,  // top right
        1.0f, 0.0f,    1.0f, 0.0f,  // bottom right
        0.0f, 0.0f,    0.0f, 0.0f   // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Renderer::drawTexturedQuad(const TextureData* texture, const RenderProperties& props) {
    if (!texture) return;

    glUseProgram(m_textureShaderProgram);

    float vertices[] = {
        // Positions    // Texture coords
        0.0f, 1.0f,    props.region.u1, props.region.v1,  // top left
        1.0f, 1.0f,    props.region.u2, props.region.v1,  // top right
        1.0f, 0.0f,    props.region.u2, props.region.v2,  // bottom right
        0.0f, 0.0f,    props.region.u1, props.region.v2   // bottom left
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    // Calculate transformation
    glm::mat4 model = calculateTransform(props);

    // Set uniforms
    GLint modelLoc = glGetUniformLocation(m_textureShaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(m_textureShaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(m_textureShaderProgram, "projection");
    GLint colorLoc = glGetUniformLocation(m_textureShaderProgram, "color");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_camera->getViewMatrix()));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(m_camera->getProjectionMatrix()));
    glUniform4fv(colorLoc, 1, glm::value_ptr(props.color));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    // Draw
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::shutdown() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteProgram(m_shaderProgram);
    glDeleteProgram(m_textureShaderProgram);
}

void Renderer::initShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // 检查着色器编译错误
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // 检查片段着色器编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // 链接着色器程序
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    // 检查链接错误
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    // 清理
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

glm::mat4 Renderer::calculateTransform(const RenderProperties& props) {
    glm::mat4 model = glm::mat4(1.0f);

    // Move to pivot point
    model = glm::translate(model,
        glm::vec3(props.position + props.pivot * props.size, 0.0f));

    // Apply rotation
    if (props.rotation != 0.0f) {
        model = glm::rotate(model, glm::radians(props.rotation),
            glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // Apply flipping
    glm::vec2 scale = props.size;
    if (props.flipX) scale.x *= -1.0f;
    if (props.flipY) scale.y *= -1.0f;

    // Move back from pivot point and apply scale
    model = glm::translate(model,
        glm::vec3(-props.pivot * props.size, 0.0f));
    model = glm::scale(model, glm::vec3(scale, 1.0f));

    return model;
}

void Renderer::drawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, float alpha) {
    glUseProgram(m_shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    GLint modelLoc = glGetUniformLocation(m_shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(m_shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(m_shaderProgram, "projection");
    GLint colorLoc = glGetUniformLocation(m_shaderProgram, "color");
    GLint alphaLoc = glGetUniformLocation(m_shaderProgram, "alpha");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_camera->getViewMatrix()));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(m_camera->getProjectionMatrix()));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));
    glUniform1f(alphaLoc, alpha);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::updateCamera(float deltaTime) {
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_MINUS)) {
        m_camera->setZoom(m_camera->getZoom() - deltaTime);
    }
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_EQUAL)) {
        m_camera->setZoom(m_camera->getZoom() + deltaTime);
    }
}

void Renderer::onWindowResize(int width, int height) {
    m_screenSize = glm::vec2(width, height);
    glViewport(0, 0, width, height);
    if (m_camera) {
        m_camera.reset(new Camera(width, height));
    }
}