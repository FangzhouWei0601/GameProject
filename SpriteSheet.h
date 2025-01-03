#pragma once
#include "TextureData.h"
#include <vector>

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

    TextureData* getTexture() const { return m_texture; }
    int getFrameCount() const { return m_regions.size(); }

private:
    void generateRegions() {
        for (int y = 0; y < m_rows; ++y) {
            for (int x = 0; x < m_columns; ++x) {
                m_regions.push_back(
                    TextureRegion::fromPixels(
                        x * m_frameWidth,
                        y * m_frameHeight,
                        m_frameWidth,
                        m_frameHeight,
                        m_texture->width,
                        m_texture->height
                    )
                );
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