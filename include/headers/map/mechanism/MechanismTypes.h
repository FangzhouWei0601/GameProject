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
    Damage,         // 伤害
    Heal,          // 治疗
    Buff,          // 增益
    Debuff,        // 减益
    MovePlatform,  // 平台移动
    OpenGate,      // 开门
    SpawnEnemy,    // 生成敌人
    Teleport      // 传送
};

struct MechanismEffect {
    EffectType type = EffectType::None;
    std::string targetId;        // 影响目标的ID
    float duration = 0.0f;       // 效果持续时间
    float value = 0.0f;         // 效果值
    glm::vec2 direction{ 0.0f };  // 效果方向（如果需要）

    // 添加效果特定参数
    struct {
        bool isPeriodic = false;     // 是否周期性生效
        float interval = 0.0f;       // 周期间隔
        float lastTriggerTime = 0.0f;// 上次触发时间
    } timing;

    struct {
        glm::vec2 startPos{ 0.0f };   // 起始位置
        glm::vec2 endPos{ 0.0f };     // 结束位置
        float speed = 0.0f;         // 移动速度
        bool isLoop = false;        // 是否循环
    } movement;
};