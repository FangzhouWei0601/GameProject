#include "SimpleJSON.h"
#include <sstream>
#include <iomanip>

namespace SimpleJSON {

    // JSONValue implementation
    JSONValue& JSONValue::operator[](const std::string& key) {
        if (!isObject()) {
            throw std::runtime_error("Value is not an object");
        }
        return (*std::get<std::shared_ptr<JSONObject>>(m_value))[key];
    }

    const JSONValue& JSONValue::operator[](const std::string& key) const {
        if (!isObject()) {
            throw std::runtime_error("Value is not an object");
        }
        return (*std::get<std::shared_ptr<JSONObject>>(m_value)).at(key);
    }

    JSONValue& JSONValue::operator[](size_t index) {
        if (!isArray()) {
            throw std::runtime_error("Value is not an array");
        }
        return (*std::get<std::shared_ptr<JSONArray>>(m_value))[index];
    }

    const JSONValue& JSONValue::operator[](size_t index) const {
        if (!isArray()) {
            throw std::runtime_error("Value is not an array");
        }
        return (*std::get<std::shared_ptr<JSONArray>>(m_value))[index];
    }

    std::string JSONValue::toString(int indent) const {
        std::ostringstream ss;
        std::string indentStr(indent, ' ');

        if (isNull()) {
            ss << "null";
        }
        else if (isBool()) {
            ss << (getBool() ? "true" : "false");
        }
        else if (isInt()) {
            ss << getInt();
        }
        else if (isDouble()) {
            ss << std::fixed << std::setprecision(6) << getDouble();
        }
        else if (isString()) {
            ss << "\"" << getString() << "\"";
        }
        else if (isArray()) {
            const auto& arr = getArray();
            ss << "[\n";
            for (size_t i = 0; i < arr.size(); ++i) {
                ss << indentStr << "  " << arr[i].toString(indent + 2);
                if (i < arr.size() - 1) ss << ",";
                ss << "\n";
            }
            ss << indentStr << "]";
        }
        else if (isObject()) {
            const auto& obj = getObject();
            ss << "{\n";
            size_t i = 0;
            for (const auto& [key, value] : obj) {
                ss << indentStr << "  \"" << key << "\": " << value.toString(indent + 2);
                if (i < obj.size() - 1) ss << ",";
                ss << "\n";
                ++i;
            }
            ss << indentStr << "}";
        }

        return ss.str();
    }

    // Parser implementation
    JSONValue JSONParser::parse(const std::string& input) {
        size_t pos = 0;
        skipWhitespace(input, pos);
        return parseValue(input, pos);
    }

    // ... [Additional parser implementation methods]
}