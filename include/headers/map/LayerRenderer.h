#pragma once
#include <vector>
#include <memory>
#include "ILayer.h"

class LayerRenderer {
public:
    void addLayer(std::unique_ptr<ILayer> layer);
    void removeLayer(ILayer* layer);
    void clear();

    void update(float deltaTime);
    void render();

    void setViewport(const glm::vec2& position, const glm::vec2& size);

private:
    std::vector<std::unique_ptr<ILayer>> m_layers;
    void sortLayers();
};