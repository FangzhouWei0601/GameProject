#pragma once
#include "IMechanism.h"
#include <vector>
#include <string>

class SequenceMechanism : public IMechanism {
public:
    SequenceMechanism(const std::string& id, const std::vector<std::string>& sequence);

    void activate() override;
    void deactivate() override;
    void update(float deltaTime) override;
    void reset() override;

    void activateTrigger(const std::string& triggerId);
    bool isSequenceComplete() const;

    size_t getCurrentStep() const { return m_currentStep; }
    float getTimeLimit() const { return m_timeLimit; }
    void setTimeLimit(float limit) { m_timeLimit = limit; }

private:
    std::vector<std::string> m_sequence;        
    std::vector<std::string> m_currentSequence;
    size_t m_currentStep;                     

    float m_timeLimit;     
    float m_timer;        

    bool validateSequence() const;
    void handleSequenceFailure();
};