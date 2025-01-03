#include "../../../include/headers/character/AttributeSystem.h"

AttributeSystem::AttributeSystem() : m_availablePoints(0) {
    // init
    for (int i = 0; i < 5; ++i) {
        auto type = static_cast<AttributeType>(i);
        m_baseAttributes[type] = 10;  // basic attribute points
        m_bonusAttributes[type] = 0;
    }
}

bool AttributeSystem::addAttributePoint(AttributeType type, int points) {
    if (points <= 0 || points > m_availablePoints) return false;
    if (!isValidAttributeChange(type, points)) return false;

    m_baseAttributes[type] += points;
    m_availablePoints -= points;
    return true;
}

bool AttributeSystem::removeAttributePoint(AttributeType type, int points) {
    if (points <= 0) return false;

    int currentPoints = m_baseAttributes[type];
    if (currentPoints - points < 10) return false;  // cant lower than init value

    m_baseAttributes[type] -= points;
    m_availablePoints += points;
    return true;
}

int AttributeSystem::getBaseAttribute(AttributeType type) const {
    auto it = m_baseAttributes.find(type);
    return it != m_baseAttributes.end() ? it->second : 0;
}

int AttributeSystem::getBonusAttribute(AttributeType type) const {
    auto it = m_bonusAttributes.find(type);
    return it != m_bonusAttributes.end() ? it->second : 0;
}

int AttributeSystem::getTotalAttribute(AttributeType type) const {
    return getBaseAttribute(type) + getBonusAttribute(type);
}

void AttributeSystem::addAttributeBonus(AttributeType type, int bonus) {
    if (bonus > 0) {
        m_bonusAttributes[type] += bonus;
    }
}

void AttributeSystem::removeAttributeBonus(AttributeType type, int bonus) {
    if (bonus > 0) {
        m_bonusAttributes[type] = std::max(0, m_bonusAttributes[type] - bonus);
    }
}

void AttributeSystem::setAvailablePoints(int points) {
    m_availablePoints = std::max(0, points);
}

bool AttributeSystem::isValidAttributeChange(AttributeType type, int points) const {
    int currentValue = getBaseAttribute(type);
    return (currentValue + points) <= BASE_ATTRIBUTE_CAP;
}