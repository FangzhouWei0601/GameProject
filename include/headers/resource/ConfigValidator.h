#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

class ConfigValidator {
public:
    static bool validateAreaConfig(const nlohmann::json& json);
    static bool validateMechanismConfig(const nlohmann::json& json);

private:
    static bool checkRequiredFields(const nlohmann::json& json,
        const std::vector<std::string>& fields);
    static bool validateTriggerMechanism(const nlohmann::json& json);
    static bool validateSequenceMechanism(const nlohmann::json& json);
};