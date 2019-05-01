#pragma once

#include <string>
#include <fstream>
#include <istream>
#include <map>
#include <filesystem>

class KeyValueParser {
public:
    KeyValueParser() = default;
    explicit KeyValueParser(const std::filesystem::path& filepath);
    explicit KeyValueParser(const std::string& str);
    explicit KeyValueParser(std::ifstream& file_input);
    explicit KeyValueParser(std::istream& input);
    KeyValueParser(const KeyValueParser& other) = default;
    KeyValueParser& operator=(const KeyValueParser& rhs) = default;
    KeyValueParser(KeyValueParser&& other) = default;
    KeyValueParser& operator=(KeyValueParser&& rhs) = default;
    ~KeyValueParser() = default;

    bool HasKey(const std::string& key) const;

    bool Parse(const std::string& input);
    bool Parse(std::ifstream& input);
    bool Parse(std::istream& input);

    //Releases the underlying database to the caller.
    [[nodiscard]] std::map<std::string, std::string>&& Release();
protected:
private:

    bool ParseMultiParams(const std::string& input);
    void ConvertFromMultiParam(std::string& whole_line);
    void CollapseMultiParamWhitespace(std::string& whole_line);
    std::size_t CountCharNotInQuotes(std::string& cur_line, char c);

    void SetValue(const std::string& key, const std::string& value);
    void SetValue(const std::string& key, const bool& value);

    std::map<std::string, std::string> _kv_pairs{};
};