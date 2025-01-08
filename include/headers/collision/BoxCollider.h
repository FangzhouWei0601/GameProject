#pragma once
#include "ICollider.h"
#include <limits>
#include <iostream>

class BoxCollider : public ICollider {
public:
    BoxCollider(const glm::vec2& pos = glm::vec2(0.0f), const glm::vec2& size = glm::vec2(1.0f));

    void setPosition(const glm::vec2& pos) override;
    void setSize(const glm::vec2& size) override;

    glm::vec2 getPosition() const override { return m_position; }
    glm::vec2 getSize() const override { return m_size; }

    bool isColliding(const ICollider* other) const override;
    CollisionManifold checkCollision(const ICollider* other) const override;

    // 获取碰撞体边界
    glm::vec2 getMin() const { return m_position; }
    glm::vec2 getMax() const { return m_position + m_size; }

    // 验证函数
    static bool isValidPosition(const glm::vec2& pos);
    static bool isValidSize(const glm::vec2& size);

private:
    glm::vec2 m_position;  // 左上角位置
    glm::vec2 m_size;      // 宽高

    // 检查碰撞体是否在有效范围内
    bool isInBounds() const;

    // 检测碰撞体是否重叠
    bool checkOverlap(const BoxCollider* other) const;

    static constexpr float MAX_POSITION = 10000.0f;  // 最大允许位置
    static constexpr float MIN_SIZE = 1.0f;         // 最小尺寸
    static constexpr float MAX_SIZE = 1000.0f;      // 最大尺寸
};