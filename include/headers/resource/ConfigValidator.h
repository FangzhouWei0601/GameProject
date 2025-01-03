#pragma once
class ConfigValidator {
public:
    static bool validateAreaConfig(const nlohmann::json& json);
    static bool validateMechanismConfig(const nlohmann::json& json);

private:
    static bool checkRequiredFields(const nlohmann::json& json,
        const std::vector<std::string>& fields);
};

// ConfigValidator.cpp
bool ConfigValidator::validateAreaConfig(const nlohmann::json& json) {
    std::vector<std::string> requiredFields = {
        "id", "name", "type", "unlocked", "bounds"
    };

    if (!checkRequiredFields(json, requiredFields)) return false;

    // ÑéÖ¤bounds×Ö¶Î
    std::vector<std::string> boundsFields = { "x", "y", "width", "height" };
    if (!checkRequiredFields(json["bounds"], boundsFields)) return false;

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