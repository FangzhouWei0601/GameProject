#pragma once
#include <unordered_map>
#include <string>

enum class AttributeType {
    Strength,      // str
    Agility,       // agi
    Vitality,      // vit
    Intelligence,  // int
    Perception     // per
};

class AttributeSystem {
public:
    static constexpr int BASE_ATTRIBUTE_CAP = 200;  // basic attribute limit

    AttributeSystem();

    // Attribute Point Allocation
    bool addAttributePoint(AttributeType type, int points = 1);
    bool removeAttributePoint(AttributeType type, int points = 1);

    // Attribute acquisition
    int getBaseAttribute(AttributeType type) const;
    int getBonusAttribute(AttributeType type) const;
    int getTotalAttribute(AttributeType type) const;

    // Equipment and Status Bonuses
    void addAttributeBonus(AttributeType type, int bonus);
    void removeAttributeBonus(AttributeType type, int bonus);

    // Attribute point management
    void setAvailablePoints(int points);
    int getAvailablePoints() const { return m_availablePoints; }

private:
    std::unordered_map<AttributeType, int> m_baseAttributes;    // Basic Attributes
    std::unordered_map<AttributeType, int> m_bonusAttributes;   // Bonus Attributes
    int m_availablePoints;

    bool isValidAttributeChange(AttributeType type, int points) const;
};