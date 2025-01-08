#pragma once
#include "../renderer/Renderer.h"
#include <string>
#include <algorithm>

class LoadingScreen {
public:
    LoadingScreen();

    void show(const std::string& loadingText = "Loading...");
    void hide();
    void update(float deltaTime);
    void render();

    // Loading progress
    void setProgress(float progress);
    float getProgress() const { return m_progress; }
    bool isVisible() const { return m_isVisible; }

    // Customization
    void setBackgroundColor(const glm::vec4& color) { m_backgroundColor = color; }
    void setProgressBarColor(const glm::vec4& color) { m_progressBarColor = color; }

private:
    bool m_isVisible;
    float m_progress;      // 0.0f to 1.0f
    std::string m_text;

    glm::vec4 m_backgroundColor;
    glm::vec4 m_progressBarColor;

    void renderBackground();
    void renderProgressBar();
    void renderText();
};