#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <stdexcept>

namespace SimpleJSON {

    class JSONValue;
    using JSONObject = std::map<std::string, JSONValue>;
    using JSONArray = std::vector<JSONValue>;
    using JSONVariant = std::variant<std::nullptr_t, bool, int, double, std::string,
        std::shared_ptr<JSONArray>, std::shared_ptr<JSONObject>>;

    class JSONValue {
    public:
        // Constructors
        JSONValue() : m_value(nullptr) {}
        JSONValue(std::nullptr_t) : m_value(nullptr) {}
        JSONValue(bool value) : m_value(value) {}
        JSONValue(int value) : m_value(value) {}
        JSONValue(double value) : m_value(value) {}
        JSONValue(const std::string& value) : m_value(value) {}
        JSONValue(const char* value) : m_value(std::string(value)) {}
        JSONValue(const JSONArray& value) : m_value(std::make_shared<JSONArray>(value)) {}
        JSONValue(const JSONObject& value) : m_value(std::make_shared<JSONObject>(value)) {}

        // Type checking
        bool isNull() const { return std::holds_alternative<std::nullptr_t>(m_value); }
        bool isBool() const { return std::holds_alternative<bool>(m_value); }
        bool isInt() const { return std::holds_alternative<int>(m_value); }
        bool isDouble() const { return std::holds_alternative<double>(m_value); }
        bool isString() const { return std::holds_alternative<std::string>(m_value); }
        bool isArray() const { return std::holds_alternative<std::shared_ptr<JSONArray>>(m_value); }
        bool isObject() const { return std::holds_alternative<std::shared_ptr<JSONObject>>(m_value); }

        // Value getters
        bool getBool() const { return std::get<bool>(m_value); }
        int getInt() const { return std::get<int>(m_value); }
        double getDouble() const { return std::get<double>(m_value); }
        const std::string& getString() const { return std::get<std::string>(m_value); }
        const JSONArray& getArray() const { return *std::get<std::shared_ptr<JSONArray>>(m_value); }
        const JSONObject& getObject() const { return *std::get<std::shared_ptr<JSONObject>>(m_value); }

        // Object operations
        JSONValue& operator[](const std::string& key);
        const JSONValue& operator[](const std::string& key) const;

        // Array operations
        JSONValue& operator[](size_t index);
        const JSONValue& operator[](size_t index) const;

        // Serialization
        std::string toString(int indent = 0) const;

    private:
        JSONVariant m_value;
    };

    // Parser class
    class JSONParser {
    public:
        static JSONValue parse(const std::string& input);

    private:
        static JSONValue parseValue(const std::string& input, size_t& pos);
        static JSONObject parseObject(const std::string& input, size_t& pos);
        static JSONArray parseArray(const std::string& input, size_t& pos);
        static std::string parseString(const std::string& input, size_t& pos);
        static JSONValue parseNumber(const std::string& input, size_t& pos);
        static void skipWhitespace(const std::string& input, size_t& pos);
    };

}