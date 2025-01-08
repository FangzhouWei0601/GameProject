#pragma once
#include <glm/glm.hpp>
#include "../resource/TextureData.h"

class ILayer {
public:
    virtual ~ILayer() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    virtual void setViewport(const glm::vec2& position, const glm::vec2& size) {
        m_viewportPosition = position;
        m_viewportSize = size;
    }

    void setZOrder(int order) { m_zOrder = order; }
    int getZOrder() const { return m_zOrder; }

protected:
    glm::vec2 m_viewportPosition{ 0.0f };
    glm::vec2 m_viewportSize{ 0.0f };
    int m_zOrder = 0;
};