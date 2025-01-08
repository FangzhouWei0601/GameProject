#pragma once
#include <glm/glm.hpp>
#include <cstdint>

// 碰撞层定义
enum class CollisionLayerBits : uint32_t {
    None = 0,
    Player = 1 << 0,  // 0x0001
    Trigger = 1 << 1,  // 0x0002
    Door = 1 << 2,  // 0x0004
    Portal = 1 << 3,  // 0x0008
    Wall = 1 << 4,  // 0x0010
    Item = 1 << 5,  // 0x0020
    Enemy = 1 << 6,  // 0x0040
    Weapon = 1 << 7,  // 0x0080
    All = 0xFFFFFFFF
};

// 预定义的碰撞掩码
struct CollisionMasks {
    static constexpr uint32_t Player =
        static_cast<uint32_t>(CollisionLayerBits::Wall) |
        static_cast<uint32_t>(CollisionLayerBits::Door) |
        static_cast<uint32_t>(CollisionLayerBits::Portal) |
        static_cast<uint32_t>(CollisionLayerBits::Item) |
        static_cast<uint32_t>(CollisionLayerBits::Enemy) |
        static_cast<uint32_t>(CollisionLayerBits::Trigger);

    static constexpr uint32_t Trigger =
        static_cast<uint32_t>(CollisionLayerBits::Player);

    static constexpr uint32_t Door =
        static_cast<uint32_t>(CollisionLayerBits::Player);

    static constexpr uint32_t Portal =
        static_cast<uint32_t>(CollisionLayerBits::Player);

    static constexpr uint32_t Wall =
        static_cast<uint32_t>(CollisionLayerBits::Player) |
        static_cast<uint32_t>(CollisionLayerBits::Enemy);

    static constexpr uint32_t Enemy =
        static_cast<uint32_t>(CollisionLayerBits::Player) |
        static_cast<uint32_t>(CollisionLayerBits::Wall);
};

struct CollisionLayer {
    uint32_t layer = static_cast<uint32_t>(CollisionLayerBits::None);
    uint32_t mask = static_cast<uint32_t>(CollisionLayerBits::None);

    CollisionLayer() = default;
    CollisionLayer(CollisionLayerBits layerBits, uint32_t maskBits)
        : layer(static_cast<uint32_t>(layerBits))
        , mask(maskBits) {
    }
};

// 碰撞结果信息
struct CollisionManifold {
    bool hasCollision = false;
    glm::vec2 normal{ 0.0f, 0.0f };
    float penetration = 0.0f;

    CollisionManifold() = default;
    explicit CollisionManifold(bool collision) : hasCollision(collision) {}
};

// 碰撞层管理类
class CollisionManager {
public:
    static CollisionManager& getInstance() {
        static CollisionManager instance;
        return instance;
    }

    void setLayerCollision(CollisionLayerBits layer1, CollisionLayerBits layer2, bool shouldCollide) {
        if (shouldCollide) {
            enableLayerCollision(layer1, layer2);
        }
        else {
            disableLayerCollision(layer1, layer2);
        }
    }

    bool canLayersCollide(uint32_t layer1, uint32_t layer2) const {
        return (layer1 & layer2) != 0;
    }

    // 获取默认碰撞层设置
    CollisionLayer getDefaultLayer(CollisionLayerBits type) const {
        switch (type) {
        case CollisionLayerBits::Player:
            return CollisionLayer(CollisionLayerBits::Player, CollisionMasks::Player);
        case CollisionLayerBits::Trigger:
            return CollisionLayer(CollisionLayerBits::Trigger, CollisionMasks::Trigger);
        case CollisionLayerBits::Door:
            return CollisionLayer(CollisionLayerBits::Door, CollisionMasks::Door);
        case CollisionLayerBits::Portal:
            return CollisionLayer(CollisionLayerBits::Portal, CollisionMasks::Portal);
        case CollisionLayerBits::Wall:
            return CollisionLayer(CollisionLayerBits::Wall, CollisionMasks::Wall);
        case CollisionLayerBits::Enemy:
            return CollisionLayer(CollisionLayerBits::Enemy, CollisionMasks::Enemy);
        default:
            return CollisionLayer();
        }
    }

private:
    CollisionManager() = default;

    void enableLayerCollision(CollisionLayerBits layer1, CollisionLayerBits layer2) {
        uint32_t bit1 = static_cast<uint32_t>(layer1);
        uint32_t bit2 = static_cast<uint32_t>(layer2);
        // 设置双向碰撞
        m_collisionMatrix[bit1] |= bit2;
        m_collisionMatrix[bit2] |= bit1;
    }

    void disableLayerCollision(CollisionLayerBits layer1, CollisionLayerBits layer2) {
        uint32_t bit1 = static_cast<uint32_t>(layer1);
        uint32_t bit2 = static_cast<uint32_t>(layer2);
        // 移除双向碰撞
        m_collisionMatrix[bit1] &= ~bit2;
        m_collisionMatrix[bit2] &= ~bit1;
    }

    uint32_t m_collisionMatrix[32] = { 0 };  // 碰撞矩阵
};