#include <algorithm>
#include "../../../include/headers/map/LayerRenderer.h"
#include <iostream>

void LayerRenderer::addLayer(std::unique_ptr<ILayer> layer) {
    if (layer) {
        m_layers.push_back(std::move(layer));
        sortLayers();
    }
}

void LayerRenderer::removeLayer(ILayer* layer) {
    auto it = std::find_if(m_layers.begin(), m_layers.end(),
        [layer](const auto& ptr) { return ptr.get() == layer; });

    if (it != m_layers.end()) {
        m_layers.erase(it);
    }
}

void LayerRenderer::clear() {
    m_layers.clear();
}

void LayerRenderer::update(float deltaTime) {
    for (auto& layer : m_layers) {
        layer->update(deltaTime);
    }
}

void LayerRenderer::render() {
    //std::cout << "Rendering layers, count: " << m_layers.size() << std::endl;
    for (auto& layer : m_layers) {
        if (layer) {
            layer->render();
        }
    }
}

void LayerRenderer::setViewport(const glm::vec2& position, const glm::vec2& size) {
    for (auto& layer : m_layers) {
        layer->setViewport(position, size);
    }
}

void LayerRenderer::sortLayers() {
    std::sort(m_layers.begin(), m_layers.end(),
        [](const auto& a, const auto& b) {
            return a->getZOrder() < b->getZOrder();
        });
}