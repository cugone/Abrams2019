#include "Engine/Core/Config.hpp"

#include "Engine/Core/ArgumentParser.hpp"
#include "Engine/Core/KeyValueParser.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <algorithm>
#include <locale>
#include <sstream>

Config::Config(KeyValueParser&& kvp) noexcept
    : _config(std::move(kvp.Release()))
{
    /* DO NOTHING */
}


Config::Config(Config&& other) noexcept
    : _config(std::move(other._config))
{
    other._config = {};
}


Config& Config::operator=(Config&& rhs) noexcept {
    _config = rhs._config;
    rhs._config = {};
    return *this;
}

bool Config::HasKey(const std::string& key) const noexcept {
    return _config.find(key) != _config.end();
}

void Config::GetValue(const std::string& key, char& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = *(found->second.begin());
    }
}

void Config::GetValue(const std::string& key, unsigned char& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = static_cast<unsigned char>(std::stoul(found->second));
    }
}

void Config::GetValue(const std::string& key, signed char& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = static_cast<signed char>(std::stoi(found->second));
    }
}

void Config::GetValue(const std::string& key, bool& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        try {
            int keyAsInt = std::stoi(found->second);
            value = keyAsInt != 0;
        } catch(...) {
            std::string keyAsString = StringUtils::ToLowerCase(found->second);
            if(keyAsString == "true") {
                value = true;
            } else if(keyAsString == "false") {
                value = false;
            } else {
                value = false;
            }
        }
    }
}

void Config::GetValue(const std::string& key, unsigned int& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = static_cast<unsigned int>(std::stoul(found->second));
    }
}

void Config::GetValue(const std::string& key, int& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stoi(found->second);
    }
}

void Config::GetValue(const std::string& key, long& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stol(found->second);
    }
}

void Config::GetValue(const std::string& key, unsigned long& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stoul(found->second);
    }
}

void Config::GetValue(const std::string& key, long long& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stoll(found->second);
    }
}

void Config::GetValue(const std::string& key, unsigned long long& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stoull(found->second);
    }
}

void Config::GetValue(const std::string& key, float& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stof(found->second);
    }
}

void Config::GetValue(const std::string& key, double& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stod(found->second);
    }
}

void Config::GetValue(const std::string& key, long double& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stold(found->second);
    }
}

void Config::GetValue(const std::string& key, std::string& value) const noexcept {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = found->second;
    }
}

void Config::SetValue(const std::string& key, const char& value) noexcept {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const unsigned char& value) noexcept {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const signed char& value) noexcept {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const bool& value) noexcept {
    _config[key] = value ? "true" : "false";
}

void Config::SetValue(const std::string& key, const unsigned int& value) noexcept {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const int& value) noexcept {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const long& value) noexcept {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const unsigned long& value) noexcept {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const long long& value) noexcept {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const unsigned long long& value) noexcept {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const float& value) noexcept {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const double& value) noexcept {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const long double& value) noexcept {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const std::string& value) noexcept {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const char* value) noexcept {
    SetValue(key, value ? std::string(value) : std::string{});
}

void Config::PrintConfigs(std::ostream& output /*= std::cout*/) const noexcept {
    for(auto iter = _config.begin(); iter != _config.end(); ++iter) {
        bool value_has_space = false;
        for(const auto& c : iter->second) {
            value_has_space |= std::isspace(c, std::locale(""));
            if(value_has_space) {
                break;
            }
        }
        output << iter->first << '=';
        if(value_has_space) {
            output << '"';
        }
        output << iter->second;
        if(value_has_space) {
            output << '"';
        }
        output << '\n';
    }
}

std::ostream& operator<<(std::ostream& output, const Config& config) noexcept {
    config.PrintConfigs(output);
    return output;
}

std::istream& operator>>(std::istream& input, Config& config) noexcept {
    KeyValueParser kvp(input);
    config._config = std::move(kvp.Release());
    return input;
}