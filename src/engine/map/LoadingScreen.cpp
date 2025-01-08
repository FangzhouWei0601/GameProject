#include "../../../include/headers/map/LoadingScreen.h"

LoadingScreen::LoadingScreen()
    : m_isVisible(false)
    , m_progress(0.0f)
    , m_backgroundColor(0.0f, 0.0f, 0.0f, 0.8f)
    , m_progressBarColor(0.0f, 0.7f, 1.0f, 1.0f) {
}

void LoadingScreen::show(const std::string& loadingText) {
    m_isVisible = true;
    m_text = loadingText;
    m_progress = 0.0f;
}

void LoadingScreen::hide() {
    m_isVisible = false;
}

void LoadingScreen::update(float deltaTime) {
    // Add any loading screen animations here
}

void LoadingScreen::render() {
    if (!m_isVisible) return;

    renderBackground();
    renderProgressBar();
    renderText();
}

void LoadingScreen::setProgress(float progress) {
    m_progress = std::clamp(progress, 0.0f, 1.0f);
}

void LoadingScreen::renderBackground() {
    auto& renderer = Renderer::getInstance();
    auto screenSize = renderer.getScreenSize();

    renderer.drawRect(glm::vec2(0.0f), screenSize,
        glm::vec3(m_backgroundColor));
}

void LoadingScreen::renderProgressBar() {
    auto& renderer = Renderer::getInstance();
    auto screenSize = renderer.getScreenSize();

    // Progress bar dimensions
    float barWidth = screenSize.x * 0.6f;
    float barHeight = 20.0f;
    glm::vec2 barPos(
        (screenSize.x - barWidth) * 0.5f,
        screenSize.y * 0.7f
    );

    // Background bar
    renderer.drawRect(barPos, glm::vec2(barWidth, barHeight),
        glm::vec3(0.2f));

    // Progress bar
    renderer.drawRect(barPos,
        glm::vec2(barWidth * m_progress, barHeight),
        glm::vec3(m_progressBarColor));
}

void LoadingScreen::renderText() {
// todo:text render
}