#pragma once

#include <string>
#include <fstream>
#include <istream>
#include <map>
#include <filesystem>

class KeyValueParser {
public:
    KeyValueParser() = default;
    explicit KeyValueParser(const std::filesystem::path& filepath) noexcept;
    explicit KeyValueParser(const std::string& str) noexcept;
    explicit KeyValueParser(std::ifstream& file_input) noexcept;
    explicit KeyValueParser(std::istream& input) noexcept;
    KeyValueParser(const KeyValueParser& other) = default;
    KeyValueParser& operator=(const KeyValueParser& rhs) = default;
    KeyValueParser(KeyValueParser&& other) = default;
    KeyValueParser& operator=(KeyValueParser&& rhs) = default;
    ~KeyValueParser() = default;

    bool HasKey(const std::string& key) const noexcept;

    bool Parse(const std::string& input) noexcept;
    bool Parse(std::ifstream& input) noexcept;
    bool Parse(std::istream& input) noexcept;

    //Releases the underlying database to the caller.
    [[nodiscard]] std::map<std::string, std::string>&& Release() noexcept;
protected:
private:

    bool ParseMultiParams(const std::string& input) noexcept;
    void ConvertFromMultiParam(std::string& whole_line) noexcept;
    void CollapseMultiParamWhitespace(std::string& whole_line) noexcept;
    std::size_t CountCharNotInQuotes(std::string& cur_line, char c) noexcept;

    void SetValue(const std::string& key, const std::string& value) noexcept;
    void SetValue(const std::string& key, const bool& value) noexcept;

    std::map<std::string, std::string> _kv_pairs{};
};