#pragma once
#include <cstdint>
#include <array>

class LevelSystem {
public:
    static constexpr int MAX_BASE_LEVEL = 50;
    static constexpr int MAX_ADVANCED_LEVEL = 11;

    LevelSystem();

    // basic level
    bool gainExperience(uint32_t exp);
    bool setBaseLevel(int level);
    int getBaseLevel() const { return m_baseLevel; }
    uint32_t getCurrentExp() const { return m_currentExp; }
    uint32_t getRequiredExp() const;

    // advanced level (dragon rider level)
    bool increaseAdvancedLevel();
    int getAdvancedLevel() const { return m_advancedLevel; }
    bool isAdvancedLevelUnlocked() const { return m_baseLevel >= 30; }  // need 30 level to unlock advanced level

    int getAttributePoints() const;

private:
    int m_baseLevel;
    int m_advancedLevel;
    uint32_t m_currentExp;

    // exp
    static constexpr float BASE_EXP = 1000.0f;  // level 1 to level 2
    static const std::array<float, 3> LEVEL_MULTIPLIERS;  // different exp multiply in different level episode

    uint32_t calculateRequiredExp(int level) const;
};