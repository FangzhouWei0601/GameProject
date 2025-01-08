#pragma once
#include "ILayer.h"

class BackgroundLayer : public ILayer {
public:
    BackgroundLayer(TextureData* texture);
    ~BackgroundLayer() override = default;

    void update(float deltaTime) override;
    void render() override;

    void setParallaxFactor(const glm::vec2& factor) { m_parallaxFactor = factor; }
    void setRepeat(bool repeat) { m_repeat = repeat; }

private:
    TextureData* m_texture;
    glm::vec2 m_parallaxFactor{ 1.0f }; // ÊÓ²îÏµÊý
    bool m_repeat = false;            

    void renderTiled();
    void renderSingle();
};