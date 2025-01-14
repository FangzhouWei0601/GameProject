#pragma once
#include "IMechanism.h"
//#include "../../../include/headers/renderer/Renderer.h"
#include "../../include/headers/renderer/Renderer.h"
#include <algorithm>

class DoorMechanism : public IMechanism {
public:
    enum class DoorState {
        Closed,         // ��ȫ�ر�
        Opening,        // ���ڴ�
        Open,          // ��ȫ��
        Closing,       // ���ڹر�
        Locked,        // ������
        Malfunction    // ����״̬
    };
    DoorMechanism(const std::string& id, const glm::vec2& position, const glm::vec2& size);
    ~DoorMechanism() override = default;

    void activate() override;
    void deactivate() override;
    void update(float deltaTime) override;
    void reset() override;
    void render();

    bool isOpen() const { return m_doorState == DoorState::Open; }
    bool isLocked() const { return m_doorState == DoorState::Locked; }
    DoorState getState() const { return m_doorState; }
    void setLocked(bool locked);

private:
    DoorState m_doorState;
    float m_transitionProgress;
    float m_transitionDuration;
    float m_openSpeed;        // �����ٶ�
    float m_closeSpeed;       // �����ٶ�

    glm::vec4 m_closedColor;
    glm::vec4 m_openColor;
    glm::vec4 m_lockedColor;

    void updateDoorState(float deltaTime);
    void updateCollider();
    void updateVisuals();
    glm::vec4 getCurrentColor() const;

    static constexpr float COLLISION_DISABLE_THRESHOLD = 0.8f;  // ��ײ�������ֵ
    static constexpr float DOOR_OPEN_SPEED = 2.0f;    // Ĭ�Ͽ����ٶ�
    static constexpr float DOOR_CLOSE_SPEED = 1.5f;   // Ĭ�Ϲ����ٶ�
};