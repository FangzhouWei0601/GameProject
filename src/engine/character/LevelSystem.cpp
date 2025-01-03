#include <cmath>
#include "../../../include/headers/character/LevelSystem.h"

const std::array<float, 3> LevelSystem::LEVEL_MULTIPLIERS = { 1.25f, 1.95f, 2.65f };

LevelSystem::LevelSystem()
    : m_baseLevel(1)
    , m_advancedLevel(0)
    , m_currentExp(0) {
}

bool LevelSystem::gainExperience(uint32_t exp) {
    if (m_baseLevel >= MAX_BASE_LEVEL) return false;

    m_currentExp += exp;
    bool leveledUp = false;

    while (m_currentExp >= getRequiredExp() && m_baseLevel < MAX_BASE_LEVEL) {
        m_currentExp -= getRequiredExp();
        m_baseLevel++;
        leveledUp = true;
    }

    return leveledUp;
}

bool LevelSystem::setBaseLevel(int level) {
    if (level < 1 || level > MAX_BASE_LEVEL) return false;
    m_baseLevel = level;
    m_currentExp = 0;
    return true;
}

uint32_t LevelSystem::getRequiredExp() const {
    return calculateRequiredExp(m_baseLevel);
}

uint32_t LevelSystem::calculateRequiredExp(int level) const {
    if (level <= 0 || level >= MAX_BASE_LEVEL) return 0;

    float multiplier;
    if (level <= 15) multiplier = LEVEL_MULTIPLIERS[0];
    else if (level <= 30) multiplier = LEVEL_MULTIPLIERS[1];
    else multiplier = LEVEL_MULTIPLIERS[2];

    return static_cast<uint32_t>(BASE_EXP * level * multiplier);
}

bool LevelSystem::increaseAdvancedLevel() {
    if (!isAdvancedLevelUnlocked() || m_advancedLevel >= MAX_ADVANCED_LEVEL)
        return false;

    m_advancedLevel++;
    return true;
}

int LevelSystem::getAttributePoints() const {
    int points = 0;

    // attribute point
    for (int i = 1; i <= m_baseLevel; ++i) {
        if (i <= 20) points += 5;        
        else if (i <= 40) points += 4;   
        else points += 3;                
    }

    // advanced level attribute point
    points += m_advancedLevel * 10;      

    return points;
}