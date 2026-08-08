#include "Config.h"
#if MQTT_ENABLED

#include <cctype>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

    std::string readConfigFile(const std::string& configFile) {
        std::ifstream file(configFile);
        if (!file) {
            throw std::runtime_error("Error opening config file: "
                                     + configFile);
        }
        return std::string((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    }

    struct JsonValue {
        enum class Type {
            Null,
            Bool,
            Number,
            String,
            Array,
            Object
        };
        Type type = Type::Null;
        bool boolean = false;
        double number = 0.0;
        std::string string;
        std::vector<JsonValue> array;
        std::map<std::string, JsonValue> object;
    };

    class JsonParser {
    public:
        explicit JsonParser(const std::string& text) : s_(text) { }

        JsonValue parse() {
            JsonValue v = parseValue();
            skipWhitespace();
            if (i_ != s_.size())
                fail("trailing characters after JSON value");
            return v;
        }

    private:
        const std::string& s_;
        std::size_t i_ = 0;

        [[noreturn]] void fail(const std::string& what) const {
            throw std::runtime_error("config JSON parse error at offset "
                                     + std::to_string(i_) + ": " + what);
        }

        void skipWhitespace() {
            while (i_ < s_.size()
                   && (s_[i_] == ' ' || s_[i_] == '\t' || s_[i_] == '\n'
                       || s_[i_] == '\r')) {
                ++i_;
            }
        }

        char peek() {
            skipWhitespace();
            if (i_ >= s_.size())
                fail("unexpected end of input");
            return s_[i_];
        }

        void expect(char c) {
            if (peek() != c)
                fail(std::string("expected '") + c + "'");
            ++i_;
        }

        bool literal(const char* lit) {
            const std::size_t n = std::strlen(lit);
            if (s_.compare(i_, n, lit) == 0) {
                i_ += n;
                return true;
            }
            return false;
        }

        JsonValue parseValue() {
            switch (peek()) {
            case '{':
                return parseObject();
            case '[':
                return parseArray();
            case '"': {
                JsonValue v;
                v.type = JsonValue::Type::String;
                v.string = parseString();
                return v;
            }
            default:
                break;
            }

            JsonValue v;
            if (literal("true")) {
                v.type = JsonValue::Type::Bool;
                v.boolean = true;
                return v;
            }
            if (literal("false")) {
                v.type = JsonValue::Type::Bool;
                v.boolean = false;
                return v;
            }
            if (literal("null")) {
                v.type = JsonValue::Type::Null;
                return v;
            }
            return parseNumber();
        }

        JsonValue parseObject() {
            JsonValue v;
            v.type = JsonValue::Type::Object;
            expect('{');
            if (peek() == '}') {
                ++i_;
                return v;
            }

            while (true) {
                if (peek() != '"')
                    fail("expected object key");
                const std::string key = parseString();
                expect(':');
                v.object[key] = parseValue();

                const char c = peek();
                if (c == ',') {
                    ++i_;
                    continue;
                }
                if (c == '}') {
                    ++i_;
                    return v;
                }
                fail("expected ',' or '}'");
            }
        }

        JsonValue parseArray() {
            JsonValue v;
            v.type = JsonValue::Type::Array;
            expect('[');
            if (peek() == ']') {
                ++i_;
                return v;
            }

            while (true) {
                v.array.push_back(parseValue());

                const char c = peek();
                if (c == ',') {
                    ++i_;
                    continue;
                }
                if (c == ']') {
                    ++i_;
                    return v;
                }
                fail("expected ',' or ']'");
            }
        }

        JsonValue parseNumber() {
            skipWhitespace();
            const std::size_t start = i_;
            if (i_ < s_.size() && (s_[i_] == '-' || s_[i_] == '+'))
                ++i_;
            while (i_ < s_.size()
                   && (std::isdigit(static_cast<unsigned char>(s_[i_]))
                       || s_[i_] == '.' || s_[i_] == 'e' || s_[i_] == 'E'
                       || s_[i_] == '+' || s_[i_] == '-')) {
                ++i_;
            }
            if (start == i_)
                fail("invalid value");

            JsonValue v;
            v.type = JsonValue::Type::Number;
            try {
                v.number = std::stod(s_.substr(start, i_ - start));
            } catch (const std::exception&) {
                fail("invalid number");
            }
            return v;
        }

        std::string parseString() {
            expect('"');
            std::string out;
            while (true) {
                if (i_ >= s_.size())
                    fail("unterminated string");
                const char c = s_[i_++];
                if (c == '"')
                    return out;
                if (c != '\\') {
                    out += c;
                    continue;
                }

                if (i_ >= s_.size())
                    fail("unterminated escape");
                switch (s_[i_++]) {
                case '"':
                    out += '"';
                    break;
                case '\\':
                    out += '\\';
                    break;
                case '/':
                    out += '/';
                    break;
                case 'b':
                    out += '\b';
                    break;
                case 'f':
                    out += '\f';
                    break;
                case 'n':
                    out += '\n';
                    break;
                case 'r':
                    out += '\r';
                    break;
                case 't':
                    out += '\t';
                    break;
                case 'u':
                    appendUtf8(parseUnicodeEscape(), out);
                    break;
                default:
                    fail("invalid escape sequence");
                }
            }
        }

        std::uint32_t parseHex4() {
            if (i_ + 4 > s_.size())
                fail("truncated \\u escape");
            std::uint32_t cp = 0;
            for (int k = 0; k < 4; ++k) {
                const char h = s_[i_++];
                cp <<= 4;
                if (h >= '0' && h <= '9')
                    cp |= static_cast<std::uint32_t>(h - '0');
                else if (h >= 'a' && h <= 'f')
                    cp |= static_cast<std::uint32_t>(h - 'a' + 10);
                else if (h >= 'A' && h <= 'F')
                    cp |= static_cast<std::uint32_t>(h - 'A' + 10);
                else
                    fail("invalid hex digit in \\u escape");
            }
            return cp;
        }

        std::uint32_t parseUnicodeEscape() {
            const std::uint32_t cp = parseHex4();
            if (cp >= 0xD800 && cp <= 0xDBFF) { // high surrogate
                if (i_ + 1 < s_.size() && s_[i_] == '\\' && s_[i_ + 1] == 'u') {
                    i_ += 2;
                    const std::uint32_t lo = parseHex4();
                    if (lo >= 0xDC00 && lo <= 0xDFFF) {
                        return 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                    }
                    fail("invalid low surrogate");
                }
                fail("unpaired high surrogate");
            }
            return cp;
        }

        static void appendUtf8(std::uint32_t cp, std::string& out) {
            if (cp < 0x80) {
                out += static_cast<char>(cp);
            } else if (cp < 0x800) {
                out += static_cast<char>(0xC0 | (cp >> 6));
                out += static_cast<char>(0x80 | (cp & 0x3F));
            } else if (cp < 0x10000) {
                out += static_cast<char>(0xE0 | (cp >> 12));
                out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                out += static_cast<char>(0x80 | (cp & 0x3F));
            } else {
                out += static_cast<char>(0xF0 | (cp >> 18));
                out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
                out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                out += static_cast<char>(0x80 | (cp & 0x3F));
            }
        }
    };

    const JsonValue& requireObject(const JsonValue& root,
                                   const std::string& name) {
        if (root.type != JsonValue::Type::Object) {
            throw std::runtime_error("config: root value must be an object");
        }
        const auto it = root.object.find(name);
        if (it == root.object.end()) {
            throw std::runtime_error("config: missing \"" + name + "\" object");
        }
        if (it->second.type != JsonValue::Type::Object) {
            throw std::runtime_error("config: \"" + name
                                     + "\" must be an object");
        }
        return it->second;
    }

    std::string requireString(const JsonValue& section,
                              const std::string& sectionName,
                              const std::string& key) {
        const auto it = section.object.find(key);
        if (it == section.object.end()) {
            throw std::runtime_error("config: missing \"" + sectionName + "."
                                     + key + "\"");
        }
        if (it->second.type != JsonValue::Type::String) {
            throw std::runtime_error("config: \"" + sectionName + "." + key
                                     + "\" must be a string");
        }
        return it->second.string;
    }

} // namespace

Config::Config(const std::string& configFile) {
    const std::string content = readConfigFile(configFile);

    JsonValue root;
    try {
        root = JsonParser(content).parse();
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse config file: " << configFile << std::endl;
        std::cerr << "exception: " << e.what() << std::endl;
        throw;
    }

    const JsonValue& mqtt = requireObject(root, "mqttConfig");
    mqttConfig.serverUrl = requireString(mqtt, "mqttConfig", "serverUrl");
    mqttConfig.clientId = requireString(mqtt, "mqttConfig", "clientId");
    mqttConfig.topic = requireString(mqtt, "mqttConfig", "topic");

    const JsonValue& ws = requireObject(root, "websocketConfig");
    websocketConfig.address = requireString(ws, "websocketConfig", "address");
    websocketConfig.port = requireString(ws, "websocketConfig", "port");
}

#endif