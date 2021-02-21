#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

class KeyValueParser;

class Config {
public:
    Config() = default;
    Config(const Config& other) = delete;
    Config& operator=(const Config& rhs) = delete;
    Config(Config&& other) noexcept;
    Config& operator=(Config&& rhs) noexcept;
    explicit Config(KeyValueParser&& kvp) noexcept;
    ~Config() = default;

    [[nodiscard]] bool LoadFromFile(const std::filesystem::path& filepath) noexcept;
    [[nodiscard]] bool AppendFromFile(const std::filesystem::path& filepath) noexcept;
    [[nodiscard]] bool AppendToFile(const std::filesystem::path& filepath) noexcept;
    [[nodiscard]] bool SaveToFile(const std::filesystem::path& filepath) noexcept;

    [[nodiscard]] bool HasKey(const std::string& key) const noexcept;

    void GetValue(const std::string& key, bool& value) const noexcept;
    void GetValue(const std::string& key, char& value) const noexcept;
    void GetValue(const std::string& key, unsigned char& value) const noexcept;
    void GetValue(const std::string& key, signed char& value) const noexcept;
    void GetValue(const std::string& key, unsigned int& value) const noexcept;
    void GetValue(const std::string& key, int& value) const noexcept;
    void GetValue(const std::string& key, long& value) const noexcept;
    void GetValue(const std::string& key, unsigned long& value) const noexcept;
    void GetValue(const std::string& key, long long& value) const noexcept;
    void GetValue(const std::string& key, unsigned long long& value) const noexcept;
    void GetValue(const std::string& key, float& value) const noexcept;
    void GetValue(const std::string& key, double& value) const noexcept;
    void GetValue(const std::string& key, long double& value) const noexcept;
    void GetValue(const std::string& key, std::string& value) const noexcept;

    void SetValue(const std::string& key, const bool& value) noexcept;
    void SetValue(const std::string& key, const char& value) noexcept;
    void SetValue(const std::string& key, const unsigned char& value) noexcept;
    void SetValue(const std::string& key, const signed char& value) noexcept;
    void SetValue(const std::string& key, const unsigned int& value) noexcept;
    void SetValue(const std::string& key, const int& value) noexcept;
    void SetValue(const std::string& key, const long& value) noexcept;
    void SetValue(const std::string& key, const unsigned long& value) noexcept;
    void SetValue(const std::string& key, const long long& value) noexcept;
    void SetValue(const std::string& key, const unsigned long long& value) noexcept;
    void SetValue(const std::string& key, const float& value) noexcept;
    void SetValue(const std::string& key, const double& value) noexcept;
    void SetValue(const std::string& key, const long double& value) noexcept;
    void SetValue(const std::string& key, const std::string& value) noexcept;
    void SetValue(const std::string& key, const char* value) noexcept;

    void PrintConfigs(std::ostream& output /*= std::cout*/) const noexcept;
    friend std::ostream& operator<<(std::ostream& output, const Config& config) noexcept;
    friend std::istream& operator>>(std::istream& input, Config& config) noexcept;

protected:
private:
    std::map<std::string, std::string> _config{};
};
