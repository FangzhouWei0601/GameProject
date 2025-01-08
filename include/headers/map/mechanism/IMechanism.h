#pragma once
#include "MechanismTypes.h"
#include "../../collision/BoxCollider.h"
#include <memory>
#include <iostream>

class IMechanism {
public:
    IMechanism(const std::string& id, MechanismType type)
        : m_id(id), m_type(type), m_state(MechanismState::Inactive) {
    }

    virtual ~IMechanism() = default;

    const std::string& getId() const { return m_id; }
    MechanismType getType() const { return m_type; }
    MechanismState getState() const { return m_state; }

    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void reset() = 0;

    void setCollider(std::unique_ptr<BoxCollider> collider) {
        m_collider = std::move(collider);
        if (m_collider) {
            std::cout << "Successfully set collider for mechanism: " << m_id << std::endl;
        }
    }
    BoxCollider* getCollider() const { return m_collider.get(); }

    bool isActive() const { return m_state == MechanismState::Active; }
    bool isFinished() const { return m_state == MechanismState::Finished; }

protected:
    std::string m_id;
    MechanismType m_type;
    MechanismState m_state;
    std::unique_ptr<BoxCollider> m_collider;

    void setState(MechanismState state) { m_state = state; }
};