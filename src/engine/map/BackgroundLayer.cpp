#include "../../../include/headers/map/BackgroundLayer.h"
#include "../../../include/headers/renderer/Renderer.h"
#include <iostream>

BackgroundLayer::BackgroundLayer(TextureData* texture)
    : m_texture(texture) {
}

void BackgroundLayer::update(float deltaTime) {
    // Background not need to update logic
    // so keep space
    return;
}

void BackgroundLayer::render() {
    if (!m_texture) {
        std::cout << "Background texture is null!" << std::endl;
        return;
    }

    //std::cout << "Rendering background: "
    //    << "Position: (" << m_viewportPosition.x << ", " << m_viewportPosition.y << ") "
    //    << "Size: (" << m_viewportSize.x << ", " << m_viewportSize.y << ")"
    //    << std::endl;

    if (m_repeat) {
        renderTiled();
    }
    else {
        renderSingle();
    }
}

void BackgroundLayer::renderTiled() {
    if (!m_texture) return;

    auto& renderer = Renderer::getInstance();

    float viewX = m_viewportPosition.x * m_parallaxFactor.x;
    float viewY = m_viewportPosition.y * m_parallaxFactor.y;

    int tilesX = static_cast<int>(std::ceil(m_viewportSize.x / m_texture->width)) + 1;
    int tilesY = static_cast<int>(std::ceil(m_viewportSize.y / m_texture->height)) + 1;

    float startX = std::floor(viewX / m_texture->width) * m_texture->width - viewX;
    float startY = std::floor(viewY / m_texture->height) * m_texture->height - viewY;

    RenderProperties props;
    props.size = glm::vec2(m_texture->width, m_texture->height);

    for (int y = 0; y < tilesY; ++y) {
        for (int x = 0; x < tilesX; ++x) {
            props.position = glm::vec2(
                startX + x * m_texture->width,
                startY + y * m_texture->height
            );
            renderer.drawTexturedQuad(m_texture, props);
        }
    }
}

void BackgroundLayer::renderSingle() {
    if (!m_texture) return;

    auto& renderer = Renderer::getInstance();

    RenderProperties props;
    props.position = -m_viewportPosition * m_parallaxFactor;
    props.size = m_viewportSize;

    renderer.drawTexturedQuad(m_texture, props);
}