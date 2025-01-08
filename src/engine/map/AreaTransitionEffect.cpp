#include "../../../include/headers/map/AreaTransitionEffect.h"
#include "../../../include/headers/renderer/Renderer.h"

AreaTransitionEffect::AreaTransitionEffect(Type type)
    : m_type(type)
    , m_duration(1.0f)
    , m_progress(0.0f)
    , m_isFinished(true) {
}

void AreaTransitionEffect::start(float duration) {
    m_duration = duration;
    m_progress = 0.0f;
    m_isFinished = false;
}

void AreaTransitionEffect::update(float deltaTime) {
    if (m_isFinished) return;

    m_progress += deltaTime / m_duration;
    if (m_progress >= 1.0f) {
        m_progress = 1.0f;
        m_isFinished = true;
    }
}

void AreaTransitionEffect::render() {
    if (m_isFinished) return;

    switch (m_type) {
    case Type::FADE:
        renderFade();
        break;
    case Type::SLIDE:
        renderSlide();
        break;
    }
}

void AreaTransitionEffect::renderFade() {
    auto& renderer = Renderer::getInstance();

    // 渲染全屏黑色矩形，使用progress作为alpha值
    glm::vec4 color(0.0f, 0.0f, 0.0f, m_progress);
    renderer.drawRect(glm::vec2(0.0f), renderer.getScreenSize(), glm::vec3(color));
}

void AreaTransitionEffect::renderSlide() {
    auto& renderer = Renderer::getInstance();
    auto screenSize = renderer.getScreenSize();

    // 从右向左滑动的效果
    float offset = screenSize.x * (1.0f - m_progress);
    renderer.drawRect(glm::vec2(offset, 0.0f), screenSize, glm::vec3(0.0f));
}