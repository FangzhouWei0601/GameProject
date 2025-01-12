#include <iostream>
#include "../../../include/headers/resource/ConfigValidator.h"

bool ConfigValidator::validateAreaConfig(const nlohmann::json& json) {
    std::vector<std::string> requiredFields = {
        "id", "name", "type", "unlocked", "bounds"
    };

    if (!checkRequiredFields(json, requiredFields)) return false;

    // Validate bounds fields
    std::vector<std::string> boundsFields = { "x", "y", "width", "height" };
    if (!checkRequiredFields(json["bounds"], boundsFields)) return false;

    return true;
}

bool ConfigValidator::validateMechanismConfig(const nlohmann::json& json) {
    if (!json.contains("mechanisms")) {
        std::cerr << "Missing mechanisms array" << std::endl;
        return false;
    }

    for (const auto& mech : json["mechanisms"]) {
        std::vector<std::string> requiredFields = {
            "id", "type"
        };

        if (!checkRequiredFields(mech, requiredFields)) return false;

        // Validate specific fields based on mechanism type
        int type = mech["type"].get<int>();
        switch (type) {
        case 0: // Trigger
            if (!validateTriggerMechanism(mech)) return false;
            break;
        case 1: // Sequence
            if (!validateSequenceMechanism(mech)) return false;
            break;
        }
    }

    return true;
}

bool ConfigValidator::checkRequiredFields(const nlohmann::json& json,
    const std::vector<std::string>& fields) {
    for (const auto& field : fields) {
        if (!json.contains(field)) {
            std::cerr << "Missing required field: " << field << std::endl;
            return false;
        }
    }
    return true;
}

bool ConfigValidator::validateTriggerMechanism(const nlohmann::json& json) {
    std::vector<std::string> requiredFields = {
        "requiresPlayer", "radius", "effectType",
        "targetId", "value", "duration"
    };
    return checkRequiredFields(json, requiredFields);
}

bool ConfigValidator::validateSequenceMechanism(const nlohmann::json& json) {
    std::vector<std::string> requiredFields = {
        "sequence"
    };
    return checkRequiredFields(json, requiredFields);
}