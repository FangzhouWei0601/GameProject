#pragma once
#include <vector>
#include "../resource/TextureData.h"
#include <memory>

class SpriteSheet {
public:
    SpriteSheet(TextureData* texture, int frameWidth, int frameHeight)
        : m_texture(texture), m_frameWidth(frameWidth), m_frameHeight(frameHeight) {
        if (texture) {
            m_columns = texture->width / frameWidth;
            m_rows = texture->height / frameHeight;
            generateRegions();
        }
    }

    TextureRegion getRegion(int frameIndex) const {
        if (frameIndex >= 0 && frameIndex < m_regions.size()) {
            return m_regions[frameIndex];
        }
        return TextureRegion();
    }

    std::unique_ptr<SpriteSheet> clone() const {
        return std::make_unique<SpriteSheet>(m_texture, m_frameWidth, m_frameHeight);
    }

    TextureData* getTexture() const { return m_texture; }
    int getFrameCount() const { return m_regions.size(); }

private:
    void generateRegions() {
        for (int y = 0; y < m_rows; ++y) {
            for (int x = 0; x < m_columns; ++x) {
                float u1 = static_cast<float>(x * m_frameWidth) / m_texture->width;
                float v1 = static_cast<float>(y * m_frameHeight) / m_texture->height;
                float u2 = static_cast<float>((x + 1) * m_frameWidth) / m_texture->width;
                float v2 = static_cast<float>((y + 1) * m_frameHeight) / m_texture->height;

                m_regions.push_back(TextureRegion(u1, v1, u2, v2));
            }
        }
    }

    TextureData* m_texture;
    int m_frameWidth;
    int m_frameHeight;
    int m_columns;
    int m_rows;
    std::vector<TextureRegion> m_regions;
};