#pragma once
#include <glm/glm.hpp>

class AreaTransitionEffect {
public:
    enum class Type {
        FADE,
        SLIDE,
        NONE
    };

    AreaTransitionEffect(Type type = Type::FADE);

    void start(float duration = 1.0f);
    void update(float deltaTime);
    void render();

    bool isFinished() const { return m_isFinished; }
    float getProgress() const { return m_progress; }

private:
    Type m_type;
    float m_duration;
    float m_progress;
    bool m_isFinished;

    void renderFade();
    void renderSlide();
};