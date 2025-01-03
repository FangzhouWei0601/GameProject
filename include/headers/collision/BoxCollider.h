#pragma once
#include "ICollider.h"

class BoxCollider : public ICollider {
public:
    BoxCollider(const glm::vec2& pos = glm::vec2(0.0f), const glm::vec2& size = glm::vec2(1.0f));

    void setPosition(const glm::vec2& pos) override { m_position = pos; }
    void setSize(const glm::vec2& size) override { m_size = size; }

    glm::vec2 getPosition() const override { return m_position; }
    glm::vec2 getSize() const override { return m_size; }

    bool isColliding(const ICollider* other) const override;
    CollisionManifold checkCollision(const ICollider* other) const override;

    // 获取碰撞箱边界
    glm::vec2 getMin() const { return m_position; }
    glm::vec2 getMax() const { return m_position + m_size; }

private:
    glm::vec2 m_position;  // 左上角位置
    glm::vec2 m_size;      // 宽高
};