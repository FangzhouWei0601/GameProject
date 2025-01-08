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

    // ��ȡ��ײ��߽�
    glm::vec2 getMin() const { return m_position; }
    glm::vec2 getMax() const { return m_position + m_size; }

    // ��֤����
    static bool isValidPosition(const glm::vec2& pos);
    static bool isValidSize(const glm::vec2& size);

private:
    glm::vec2 m_position;  // ���Ͻ�λ��
    glm::vec2 m_size;      // ���

    // �����ײ���Ƿ�����Ч��Χ��
    bool isInBounds() const;

    // �����ײ���Ƿ��ص�
    bool checkOverlap(const BoxCollider* other) const;

    static constexpr float MAX_POSITION = 10000.0f;  // �������λ��
    static constexpr float MIN_SIZE = 1.0f;         // ��С�ߴ�
    static constexpr float MAX_SIZE = 1000.0f;      // ���ߴ�
};