#include "../../../../include/headers/map/mechanism/SequenceMechanism.h"

SequenceMechanism::SequenceMechanism(const std::string& id,
    const std::vector<std::string>& sequence)
    : IMechanism(id, MechanismType::Sequence)
    , m_sequence(sequence)
    , m_currentStep(0)
    , m_timeLimit(0.0f)
    , m_timer(0.0f) {
}

void SequenceMechanism::activate() {
    if (m_state != MechanismState::Active) {
        m_state = MechanismState::Active;
        m_timer = 0.0f;
    }
}

void SequenceMechanism::deactivate() {
    if (m_state == MechanismState::Active) {
        m_state = MechanismState::Inactive;
        handleSequenceFailure();
    }
}

void SequenceMechanism::update(float deltaTime) {
    if (m_state != MechanismState::Active) return;

    if (m_timeLimit > 0.0f) {
        m_timer += deltaTime;
        if (m_timer >= m_timeLimit) {
            handleSequenceFailure();
            return;
        }
    }

    if (isSequenceComplete()) {
        m_state = MechanismState::Finished;
    }
}

void SequenceMechanism::reset() {
    m_currentSequence.clear();
    m_currentStep = 0;
    m_timer = 0.0f;
    m_state = MechanismState::Inactive;
}

void SequenceMechanism::activateTrigger(const std::string& triggerId) {
    if (m_state != MechanismState::Active) return;

    if (m_currentStep < m_sequence.size() &&
        m_sequence[m_currentStep] == triggerId) {
        m_currentSequence.push_back(triggerId);
        m_currentStep++;

        if (isSequenceComplete()) {
            m_state = MechanismState::Finished;
        }
    }
    else {
        handleSequenceFailure();
    }
}

bool SequenceMechanism::isSequenceComplete() const {
    return m_currentSequence.size() == m_sequence.size() &&
        validateSequence();
}

bool SequenceMechanism::validateSequence() const {
    if (m_currentSequence.size() != m_sequence.size()) return false;

    for (size_t i = 0; i < m_sequence.size(); ++i) {
        if (m_currentSequence[i] != m_sequence[i]) return false;
    }
    return true;
}

void SequenceMechanism::handleSequenceFailure() {
    m_currentSequence.clear();
    m_currentStep = 0;

    m_state = MechanismState::Inactive;
}