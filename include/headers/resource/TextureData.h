#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

struct TextureRegion {
    float u1, v1;    // Top-left UV coordinates
    float u2, v2;    // Bottom-right UV coordinates

    TextureRegion() : u1(0.0f), v1(0.0f), u2(1.0f), v2(1.0f) {}

    TextureRegion(float u1, float v1, float u2, float v2)
        : u1(u1), v1(v1), u2(u2), v2(v2) {
    }

    // Helper to create region from pixel coordinates
    static TextureRegion fromPixels(int x, int y, int width, int height, int textureWidth, int textureHeight) {
        return TextureRegion(
            static_cast<float>(x) / textureWidth,
            static_cast<float>(y) / textureHeight,
            static_cast<float>(x + width) / textureWidth,
            static_cast<float>(y + height) / textureHeight
        );
    }
};

struct TextureData {
    GLuint id;
    int width;
    int height;
    int channels;
    std::string name;

    TextureData() : id(0), width(0), height(0), channels(0) {}
};