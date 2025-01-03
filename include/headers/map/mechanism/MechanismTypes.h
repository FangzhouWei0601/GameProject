#pragma once
#include <functional>
#include <string>
#include <glm/glm.hpp>

enum class MechanismType {
    Trigger,        
    Sequence,       
    Breakable,      
    MovingPlatform, 
    Trap,          
    BuffZone      
};

enum class MechanismState {
    Inactive,   
    Active,     
    Finished,   
    Broken      
};


struct TriggerCondition {
    bool requiresPlayerPresence = false;  
    bool requiresInteraction = false;     
    float triggerRadius = 0.0f;          


    std::function<bool()> customCondition = nullptr;
};


enum class EffectType {
    None,
    Damage,         // �˺�
    Heal,          // ����
    Buff,          // ����
    Debuff,        // ����
    MovePlatform,  // ƽ̨�ƶ�
    OpenGate,      // ����
    SpawnEnemy,    // ���ɵ���
    Teleport      // ����
};

struct MechanismEffect {
    EffectType type = EffectType::None;
    std::string targetId;        // Ӱ��Ŀ���ID
    float duration = 0.0f;       // Ч������ʱ��
    float value = 0.0f;         // Ч��ֵ
    glm::vec2 direction{ 0.0f };  // Ч�����������Ҫ��

    // ���Ч���ض�����
    struct {
        bool isPeriodic = false;     // �Ƿ���������Ч
        float interval = 0.0f;       // ���ڼ��
        float lastTriggerTime = 0.0f;// �ϴδ���ʱ��
    } timing;

    struct {
        glm::vec2 startPos{ 0.0f };   // ��ʼλ��
        glm::vec2 endPos{ 0.0f };     // ����λ��
        float speed = 0.0f;         // �ƶ��ٶ�
        bool isLoop = false;        // �Ƿ�ѭ��
    } movement;
};