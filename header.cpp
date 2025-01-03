#pragma once
#include <memory>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

    GLFWwindow* m_window;
    bool m_isRunning;
    float m_lastFrame;
};