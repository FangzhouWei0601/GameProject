#pragma once
#include "InputTypes.h"
#include <unordered_map>
#include <vector>

class InputMapper {
public:
    static InputMapper& getInstance() {
        static InputMapper instance;
        return instance;
    }

    // Binding management
    void bindAction(GameAction action, const InputBinding& binding);
    void unbindAction(GameAction action);
    void clearAllBindings();

    // Action state queries
    bool isActionTriggered(GameAction action) const;
    bool isActionHeld(GameAction action) const;
    glm::vec2 getActionAxis(GameAction action) const;

    // Load/Save bindings
    //void loadBindings(const std::string& filename);
    //void saveBindings(const std::string& filename) const;

    // Get current bindings
    //std::vector<std::pair<GameAction, InputBinding>> getAllBindings() const;

    void setupDefaultBindings();
private:
    InputMapper() = default;
    ~InputMapper() = default;
    InputMapper(const InputMapper&) = delete;
    InputMapper& operator=(const InputMapper&) = delete;

    // Binding storage
    std::unordered_map<GameAction, std::vector<InputBinding>> m_bindings;

    // Helper functions
    bool checkBinding(const InputBinding& binding) const;
    bool checkCombination(const InputBinding& binding) const;
};

