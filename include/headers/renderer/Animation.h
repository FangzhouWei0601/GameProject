#pragma once
#include "SpriteSheet.h"
#include <vector>
#include <string>
#include <map>
#include <memory>

struct AnimationFrame {
    int frameIndex;
    float duration;

    AnimationFrame(int index, float time)
        : frameIndex(index), duration(time) {
    }
};

class Animation {
public:
    Animation(const std::string& name)
        : m_name(name), m_isLooping(true), m_currentTime(0.0f), m_currentFrameIndex(0) {
    }

    void addFrame(int frameIndex, float duration) {
        m_frames.emplace_back(frameIndex, duration);
    }

    void update(float deltaTime);
    void reset() { m_currentTime = 0.0f; m_currentFrameIndex = 0; }

    TextureRegion getCurrentRegion(const SpriteSheet& spriteSheet) const;
    bool isFinished() const { return !m_isLooping && m_currentFrameIndex >= m_frames.size(); }

    void setLooping(bool loop) { m_isLooping = loop; }
    const std::string& getName() const { return m_name; }

private:
    std::string m_name;
    std::vector<AnimationFrame> m_frames;
    bool m_isLooping;
    float m_currentTime;
    size_t m_currentFrameIndex;
};

class AnimationController {
public:
    void addAnimation(const std::string& name, std::unique_ptr<Animation> animation) {
        m_animations[name] = std::move(animation);
    }

    void playAnimation(const std::string& name);
    void update(float deltaTime);
    TextureRegion getCurrentRegion(const SpriteSheet& spriteSheet) const;

    const std::string& getCurrentAnimationName() const { return m_currentAnimationName; }
    bool hasAnimation() const { return m_currentAnimation != nullptr; }

private:
    std::map<std::string, std::unique_ptr<Animation>> m_animations;
    Animation* m_currentAnimation = nullptr;
    std::string m_currentAnimationName;
};