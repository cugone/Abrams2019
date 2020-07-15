#include "Engine/Core/ArgumentParser.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

#include <sstream>
#include <type_traits>

ArgumentParser::ArgumentParser(const std::string& args) noexcept
: _current(args) {
    /* DO NOTHING */
}

void ArgumentParser::clear() noexcept {
    _state_bits.reset();
}

bool ArgumentParser::fail() const noexcept {
    bool badbit = _state_bits[static_cast<std::size_t>(ArgumentParserState::BadBit)];
    bool failbit = _state_bits[static_cast<std::size_t>(ArgumentParserState::FailBit)];
    return badbit || failbit;
}

bool ArgumentParser::good() const noexcept {
    bool eofbit = _state_bits[static_cast<std::size_t>(ArgumentParserState::EndOfFileBit)];
    bool failbit = _state_bits[static_cast<std::size_t>(ArgumentParserState::FailBit)];
    bool badbit = _state_bits[static_cast<std::size_t>(ArgumentParserState::BadBit)];
    return !eofbit && !failbit && !badbit;
}

bool ArgumentParser::bad() const noexcept {
    bool badbit = _state_bits[static_cast<std::size_t>(ArgumentParserState::BadBit)];
    return badbit;
}

bool ArgumentParser::eof() const noexcept {
    bool eofbit = _state_bits[static_cast<std::size_t>(ArgumentParserState::EndOfFileBit)];
    return eofbit;
}

ArgumentParser::operator bool() const noexcept {
    return !(fail() || bad()) && (good() || eof());
}

bool ArgumentParser::operator!() const noexcept {
    return !operator bool();
}

bool ArgumentParser::GetNext(Rgba& value) const noexcept {
    std::string value_str{};
    if(GetNext(value_str)) {
        value = Rgba(value_str);
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(Vector2& value) const noexcept {
    std::string value_str{};
    if(GetNext(value_str)) {
        value = Vector2(value_str);
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(Vector3& value) const noexcept {
    std::string value_str{};
    if(GetNext(value_str)) {
        value = Vector3(value_str);
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(Vector4& value) const noexcept {
    std::string value_str{};
    if(GetNext(value_str)) {
        value = Vector4(value_str);
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(IntVector2& value) const noexcept {
    std::string value_str{};
    if(GetNext(value_str)) {
        value = IntVector2(value_str);
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(IntVector3& value) const noexcept {
    std::string value_str{};
    if(GetNext(value_str)) {
        value = IntVector3(value_str);
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(IntVector4& value) const noexcept {
    std::string value_str{};
    if(GetNext(value_str)) {
        value = IntVector4(value_str);
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(Matrix4& value) const noexcept {
    std::string value_str{};
    if(GetNext(value_str)) {
        value = Matrix4(value_str);
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(std::string& value) const noexcept {
    if(_current.empty()) {
        SetState(ArgumentParserState::EndOfFileBit, true);
        return false;
    }
    if(GetNextValueFromBuffer(value)) {
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}
bool ArgumentParser::GetNext(bool& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<bool>(std::stoul(value_str));
        } catch(std::invalid_argument& /*e*/) {
            value_str = StringUtils::ToLowerCase(value_str);
            if(value_str == "true") {
                value = true;
                return true;
            }
            if(value_str == "false") {
                value = false;
                return true;
            }
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}
bool ArgumentParser::GetNext(unsigned char& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<unsigned char>(std::stoul(value_str));
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(signed char& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<signed char>(std::stoi(value_str));
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(char& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        value = *value_str.begin();
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(unsigned short& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<unsigned short>(std::stoul(value_str));
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(short& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<short>(std::stoi(value_str));
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(unsigned int& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<unsigned int>(std::stoul(value_str));
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(int& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stoi(value_str);
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(unsigned long& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stoul(value_str);
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(long& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stol(value_str);
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(unsigned long long& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stoull(value_str);
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(long long& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stoll(value_str);
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(float& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stof(value_str);
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(double& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stod(value_str);
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

bool ArgumentParser::GetNext(long double& value) const noexcept {
    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stold(value_str);
        } catch(std::invalid_argument& /*e*/) {
            SetState(ArgumentParserState::BadBit, true);
            return false;
        }
        return true;
    }
    SetState(ArgumentParserState::BadBit, true);
    return false;
}

void ArgumentParser::SetState(const ArgumentParserState& stateBits, bool newValue) const noexcept {
    if((stateBits & ArgumentParserState::BadBit) == ArgumentParserState::BadBit) {
        _state_bits[static_cast<std::size_t>(ArgumentParserState::BadBit)] = newValue;
    } else if((stateBits & ArgumentParserState::FailBit) == ArgumentParserState::FailBit) {
        _state_bits[static_cast<std::size_t>(ArgumentParserState::FailBit)] = newValue;
    } else if((stateBits & ArgumentParserState::EndOfFileBit) == ArgumentParserState::EndOfFileBit) {
        _state_bits[static_cast<std::size_t>(ArgumentParserState::EndOfFileBit)] = newValue;
    }
}

bool ArgumentParser::GetNextValueFromBuffer(std::string& value) const noexcept {
    std::istringstream ss;
    ss.str(_current);
    std::string arg;
    if(!(ss >> arg)) {
        return false;
    }
    bool inQuote = false;
    if(arg.front() == '"') {
        inQuote = true;
    }
    if(arg.back() == '"') {
        inQuote = false;
        arg.erase(0, 1);
        arg.pop_back();
    }
    if(inQuote) {
        std::string next;
        while(ss >> next) {
            arg += " ";
            arg += next;
            if(next.back() == '"') {
                arg.erase(0, 1);
                arg.pop_back();
                inQuote = false;
                break;
            }
        }
    }
    if(!std::getline(ss, _current)) {
        _current = std::string("");
    }
    value = arg;
    return true;
}
