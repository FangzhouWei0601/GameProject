#include "Animation.h"

void Animation::update(float deltaTime) {
    if (m_frames.empty() || isFinished()) return;

    m_currentTime += deltaTime;

    while (m_currentTime >= m_frames[m_currentFrameIndex].duration) {
        m_currentTime -= m_frames[m_currentFrameIndex].duration;
        m_currentFrameIndex++;

        if (m_currentFrameIndex >= m_frames.size()) {
            if (m_isLooping) {
                m_currentFrameIndex = 0;
            }
            else {
                m_currentFrameIndex = m_frames.size();
                break;
            }
        }
    }
}

TextureRegion Animation::getCurrentRegion(const SpriteSheet& spriteSheet) const {
    if (m_frames.empty() || m_currentFrameIndex >= m_frames.size()) {
        return TextureRegion();
    }
    return spriteSheet.getRegion(m_frames[m_currentFrameIndex].frameIndex);
}

void AnimationController::playAnimation(const std::string& name) {
    if (m_currentAnimationName == name) return;

    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
        m_currentAnimation = it->second.get();
        m_currentAnimationName = name;
        m_currentAnimation->reset();
    }
}

void AnimationController::update(float deltaTime) {
    if (m_currentAnimation) {
        m_currentAnimation->update(deltaTime);
    }
}

TextureRegion AnimationController::getCurrentRegion(const SpriteSheet& spriteSheet) const {
    if (m_currentAnimation) {
        return m_currentAnimation->getCurrentRegion(spriteSheet);
    }
    return TextureRegion();
}