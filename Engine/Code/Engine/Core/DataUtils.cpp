#include "Engine/Core/DataUtils.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Profiling/ProfileLogScope.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

namespace DataUtils {

    void ValidateXmlElement(const XMLElement& element,
        const std::string& name,
        const std::string& requiredChildElements,
        const std::string& requiredAttributes,
        const std::string& optionalChildElements /*= std::string("")*/,
        const std::string& optionalAttributes /*= std::string("")*/) noexcept {

        if(name.empty()) {
            std::ostringstream err_ss;
            err_ss << "Element validation failed. Element name is required.";
            ERROR_AND_DIE(err_ss.str().c_str());
        }
        auto xmlNameAsCStr = element.Name();
        std::string xml_name = xmlNameAsCStr ? xmlNameAsCStr : "";
        if(xml_name != name) {
            std::ostringstream err_ss;
            err_ss << "Element validation failed. Element name \"" << xml_name << "\" does not match valid name \"" << name << "\"\n";
            ERROR_AND_DIE(err_ss.str().c_str());
        }

        //Get list of required/optional attributes/children
        //Sort
        //Remove duplicates
        //Rational for not using std:set:
        //Profiled code takes average of 10 microseconds to complete.
        std::vector<std::string> requiredAttributeNames = StringUtils::Split(requiredAttributes);
        std::sort(requiredAttributeNames.begin(), requiredAttributeNames.end());
        requiredAttributeNames.erase(std::unique(requiredAttributeNames.begin(), requiredAttributeNames.end()), requiredAttributeNames.end());

        std::vector<std::string> requiredChildElementNames = StringUtils::Split(requiredChildElements);
        std::sort(requiredChildElementNames.begin(), requiredChildElementNames.end());
        requiredChildElementNames.erase(std::unique(requiredChildElementNames.begin(), requiredChildElementNames.end()), requiredChildElementNames.end());

        std::vector<std::string> optionalChildElementNames = StringUtils::Split(optionalChildElements);
        std::sort(optionalChildElementNames.begin(), optionalChildElementNames.end());
        optionalChildElementNames.erase(std::unique(optionalChildElementNames.begin(), optionalChildElementNames.end()), optionalChildElementNames.end());

        std::vector<std::string> optionalAttributeNames = StringUtils::Split(optionalAttributes);
        std::sort(optionalAttributeNames.begin(), optionalAttributeNames.end());
        optionalAttributeNames.erase(std::unique(optionalAttributeNames.begin(), optionalAttributeNames.end()), optionalAttributeNames.end());

        std::vector<std::string> actualChildElementNames = GetChildElementNames(element);
        std::sort(actualChildElementNames.begin(), actualChildElementNames.end());
        actualChildElementNames.erase(std::unique(actualChildElementNames.begin(), actualChildElementNames.end()), actualChildElementNames.end());

        std::vector<std::string> actualAttributeNames = GetAttributeNames(element);
        std::sort(actualAttributeNames.begin(), actualAttributeNames.end());
        actualAttributeNames.erase(std::unique(actualAttributeNames.begin(), actualAttributeNames.end()), actualAttributeNames.end());

        //Difference between actual attribute names and required list is list of actual optional attributes.
        std::vector<std::string> actualOptionalAttributeNames;
        std::set_difference(actualAttributeNames.begin(), actualAttributeNames.end(),
            requiredAttributeNames.begin(), requiredAttributeNames.end(),
            std::back_inserter(actualOptionalAttributeNames)
        );
        std::sort(actualOptionalAttributeNames.begin(), actualOptionalAttributeNames.end());

        //Difference between actual child names and required list is list of actual optional children.
        std::vector<std::string> actualOptionalChildElementNames;
        std::set_difference(actualChildElementNames.begin(), actualChildElementNames.end(),
            requiredChildElementNames.begin(), requiredChildElementNames.end(),
            std::back_inserter(actualOptionalChildElementNames)
        );
        std::sort(actualOptionalChildElementNames.begin(), actualOptionalChildElementNames.end());

        //Find missing attributes
        std::vector<std::string> missingRequiredAttributes;
        std::set_difference(requiredAttributeNames.begin(), requiredAttributeNames.end(),
            actualAttributeNames.begin(), actualAttributeNames.end(),
            std::back_inserter(missingRequiredAttributes));

        if(!missingRequiredAttributes.empty()) {
            std::ostringstream err_ss;
            for(auto& c : missingRequiredAttributes) {
                err_ss << "Attribute validation failed. Missing required attribute \"" << c << "\"\n";
            }
            ERROR_AND_DIE(err_ss.str().c_str());
        }

        //Find missing children
        std::vector<std::string> missingRequiredChildren;
        std::set_difference(requiredChildElementNames.begin(), requiredChildElementNames.end(),
            actualChildElementNames.begin(), actualChildElementNames.end(),
            std::back_inserter(missingRequiredChildren));

        if(!missingRequiredChildren.empty()) {
            std::ostringstream err_ss;
            for(auto& c : missingRequiredChildren) {
                err_ss << "Child Element validation failed. Missing required child \"" << c << "\"\n";
            }
            ERROR_AND_DIE(err_ss.str().c_str());
        }

#ifdef _DEBUG
        //Find extra attributes
        std::vector<std::string> extraOptionalAttributes;
        std::set_difference(actualOptionalAttributeNames.begin(), actualOptionalAttributeNames.end(),
            optionalAttributeNames.begin(), optionalAttributeNames.end(),
            std::back_inserter(extraOptionalAttributes));

        if(!extraOptionalAttributes.empty()) {
            std::ostringstream err_ss;
            err_ss << "\n" << __FUNCTION__ << ": Optional Attribute validation failed. Verify attributes are correct. Found unknown attributes:\n";
            for(auto& c : extraOptionalAttributes) {
                err_ss << "\t\"" << c << "\"\n";
            }
            DebuggerPrintf(err_ss.str().c_str());
        }

        //Find extra children
        std::vector<std::string> extraOptionalChildren;
        std::set_difference(actualOptionalChildElementNames.begin(), actualOptionalChildElementNames.end(),
            optionalChildElementNames.begin(), optionalChildElementNames.end(),
            std::back_inserter(extraOptionalChildren));

        if(!extraOptionalChildren.empty()) {
            std::ostringstream err_ss;
            err_ss << "\n" << __FUNCTION__ << ": Optional Child validation failed. Verify attributes are correct. Found unknown children:\n";
            for(auto& c : extraOptionalChildren) {
                err_ss << "\t\"" << c << "\"\n";
            }
            DebuggerPrintf(err_ss.str().c_str());
        }
#endif //#if _DEBUG
    }

    std::size_t GetAttributeCount(const XMLElement &element) noexcept {
        std::size_t attributeCount = 0u;
        ForEachAttribute(element,
            [&](const XMLAttribute& /*attribute*/) {
            ++attributeCount;
        });
        return attributeCount;
    }

    std::vector<std::string> GetAttributeNames(const XMLElement& element) noexcept {
        std::vector<std::string> attributeNames{};
        attributeNames.reserve(GetAttributeCount(element));
        ForEachAttribute(element,
            [&](const XMLAttribute& attribute) {
            attributeNames.emplace_back(attribute.Name());
        });
        return attributeNames;
    }

    bool HasAttribute(const XMLElement& element) noexcept {
        return GetAttributeCount(element) != 0;
    }

    bool HasAttribute(const XMLElement& element, const std::string& name) {
        bool result = false;
        ForEachAttribute(element, [&name, &result](const XMLAttribute& attribute) {
            if(attribute.Name() == name) result = true;
        });
        return result;
    }

    std::size_t GetChildElementCount(const XMLElement &element, const std::string& elementName /*= std::string("")*/) noexcept {
        std::size_t childCount = 0u;
        ForEachChildElement(element, elementName,
            [&](const XMLElement& /*elem*/) {
            ++childCount;
        });
        return childCount;
    }

    std::vector<std::string> GetChildElementNames(const XMLElement& element) noexcept {
        std::vector<std::string> childElementNames{};
        childElementNames.reserve(GetChildElementCount(element));
        ForEachChildElement(element, std::string{},
            [&](const XMLElement& elem) {
            childElementNames.emplace_back(elem.Name());
        });
        return childElementNames;
    }

    bool ParseXmlElementText(const XMLElement& element, bool defaultValue) noexcept {
        bool retVal = defaultValue;
        element.QueryBoolText(&retVal);
        return retVal;
    }
    unsigned char ParseXmlElementText(const XMLElement& element, unsigned char defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoul(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoul(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoul(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoul(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomIntInRange(lower, upper));
            }
        }
        return retVal;
    }
    signed char ParseXmlElementText(const XMLElement& element, signed char defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoi(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoi(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoi(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoi(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomIntInRange(lower, upper));
            }
        }
        return retVal;
    }
    char ParseXmlElementText(const XMLElement& element, char defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoi(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoi(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoi(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoi(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomIntInRange(lower, upper));
            }
        }
        return retVal;
    }
    unsigned short ParseXmlElementText(const XMLElement& element, unsigned short defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoul(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoul(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoul(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoul(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomIntInRange(lower, upper));
            }
        }
        return retVal;
    }
    short ParseXmlElementText(const XMLElement& element, short defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoi(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoi(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoi(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoi(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomIntInRange(lower, upper));
            }
        }
        return retVal;
    }
    unsigned int ParseXmlElementText(const XMLElement& element, unsigned int defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoul(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoul(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoul(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoul(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomLongInRange(lower, upper));
            }
        }
        return retVal;
    }
    int ParseXmlElementText(const XMLElement& element, int defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoi(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoi(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoi(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoi(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomIntInRange(lower, upper));
            }
        }
        return retVal;
    }
    unsigned long ParseXmlElementText(const XMLElement& element, unsigned long defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoul(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoul(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoul(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoul(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomLongLongInRange(lower, upper));
            }
        }
        return retVal;
    }
    long ParseXmlElementText(const XMLElement& element, long defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoll(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoll(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoll(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoll(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomLongLongInRange(lower, upper));
            }
        }
        return retVal;
    }
    unsigned long long ParseXmlElementText(const XMLElement& element, unsigned long long defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoull(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoull(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoull(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoull(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomLongLongInRange(lower, upper));
            }
        }
        return retVal;
    }
    long long ParseXmlElementText(const XMLElement& element, long long defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stoll(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stoll(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stoll(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stoll(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomLongLongInRange(lower, upper));
            }
        }
        return retVal;
    }
    float ParseXmlElementText(const XMLElement& element, float defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stof(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stof(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stof(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stof(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomFloatInRange(lower, upper));
            }
        }
        return retVal;
    }
    double ParseXmlElementText(const XMLElement& element, double defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stod(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stod(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stod(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stod(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomDoubleInRange(lower, upper));
            }
        }
        return retVal;
    }

    long double ParseXmlElementText(const XMLElement& element, long double defaultValue) noexcept {
        auto retVal = defaultValue;
        auto txtAsCStr = element.GetText();
        std::string txt(txtAsCStr ? txtAsCStr : "");
        auto is_range = txt.find('~') != std::string::npos;
        if(!is_range) {
            try {
                retVal = static_cast<decltype(retVal)>(std::stold(txt));
            } catch(...) {
                return defaultValue;
            }
        } else {
            auto values = StringUtils::Split(txt, '~');
            if(values.size() == 1) {
                retVal = static_cast<decltype(retVal)>(std::stold(values[0]));
            } else {
                auto lower = static_cast<decltype(retVal)>(std::stold(values[0]));
                auto upper = static_cast<decltype(retVal)>(std::stold(values[1]));
                retVal = static_cast<decltype(retVal)>(MathUtils::GetRandomLongDoubleInRange(lower, upper));
            }
        }
        return retVal;
    }

    Rgba ParseXmlElementText(const XMLElement& element, const Rgba& defaultValue) noexcept {
        auto s = element.GetText();
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return Rgba(textVal);
        }
    }
    Vector2 ParseXmlElementText(const XMLElement& element, const Vector2& defaultValue) noexcept {
        auto s = element.GetText();
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return Vector2(textVal);
        }
    }
    IntVector2 ParseXmlElementText(const XMLElement& element, const IntVector2& defaultValue) noexcept {
        auto s = element.GetText();
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return IntVector2(textVal);
        }
    }
    Vector3 ParseXmlElementText(const XMLElement& element, const Vector3& defaultValue) noexcept {
        auto s = element.GetText();
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return Vector3(textVal);
        }
    }
    IntVector3 ParseXmlElementText(const XMLElement& element, const IntVector3& defaultValue) noexcept {
        auto s = element.GetText();
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return IntVector3(textVal);
        }
    }
    Vector4 ParseXmlElementText(const XMLElement& element, const Vector4& defaultValue) noexcept {
        auto s = element.GetText();
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return Vector4(textVal);
        }
    }
    IntVector4 ParseXmlElementText(const XMLElement& element, const IntVector4& defaultValue) noexcept {
        auto s = element.GetText();
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return IntVector4(textVal);
        }
    }
    Matrix4 ParseXmlElementText(const XMLElement& element, const Matrix4& defaultValue) noexcept {
        auto s = element.GetText();
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return Matrix4(textVal);
        }
    }

    std::string ParseXmlElementText(const XMLElement& element, const char* defaultValue) noexcept {
        std::string textVal_default(defaultValue ? defaultValue : "");
        return ParseXmlElementText(element, textVal_default);
    }

    std::string ParseXmlElementText(const XMLElement& element, const std::string& defaultValue) noexcept {
        auto s = element.GetText();
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return textVal;
        }
    }

    bool ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, bool defaultValue) noexcept {
        bool retVal = defaultValue;
        element.QueryBoolAttribute(attributeName.c_str(), &retVal);
        return retVal;
    }

    unsigned char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned char defaultValue) noexcept {
        unsigned int retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return static_cast<unsigned char>(std::stoul(values[0]));
            }
            unsigned char lower = static_cast<unsigned char>(std::stoul(values[0]));
            unsigned char upper = static_cast<unsigned char>(std::stoul(values[1]));
            retVal = MathUtils::GetRandomIntInRange(lower, upper);
        }
        return static_cast<unsigned char>(retVal);
    }

    signed char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, signed char defaultValue) noexcept {
        signed int retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryIntAttribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return static_cast<signed char>(std::stoul(values[0]));
            }
            signed char lower = static_cast<signed char>(std::stoul(values[0]));
            signed char upper = static_cast<signed char>(std::stoul(values[1]));
            retVal = MathUtils::GetRandomIntInRange(lower, upper);
        }
        return static_cast<signed char>(retVal);
    }

    char ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, char defaultValue) noexcept {
        char retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            auto attrValue = element.Attribute(attributeName.c_str());
            std::string value(attrValue ? attrValue : "");
            if(value.empty()) {
                return retVal;
            }
            return *(value.begin());
        } else {
            auto values = StringUtils::Split(attr, '~');
            //attr string isn't empty, but if Split returns empty vector, the only thing the string contained was a '~'.
            if(values.empty()) {
                return '~';
            }
            if(values.size() == 1) {
                if(attr.front() == '~') {
                    auto lower = static_cast<char>(-128);
                    auto upper = static_cast<char>(std::stoi(values[1]));
                    retVal = static_cast<char>(MathUtils::GetRandomIntInRange(lower, upper));
                }
                if(attr.back()  == '~') {
                    auto lower = static_cast<char>(std::stoi(values[0]));
                    auto upper = 128;
                    retVal = static_cast<char>(MathUtils::GetRandomIntInRange(lower, upper));
                }
                return static_cast<char>(std::stoi(values[0]));
            }
            auto lower = static_cast<char>(std::stoi(values[0]));
            auto upper = static_cast<char>(std::stoi(values[1]));
            retVal = static_cast<char>(MathUtils::GetRandomIntInRange(lower, upper));
        }
        return static_cast<char>(retVal);
    }

    unsigned short ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned short defaultValue) noexcept {
        unsigned int retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return static_cast<unsigned short>(std::stoul(values[0]));
            }
            unsigned short lower = static_cast<unsigned short>(std::stoul(values[0]));
            unsigned short upper = static_cast<unsigned short>(std::stoul(values[1]));
            retVal = MathUtils::GetRandomIntInRange(lower, upper);
        }
        return static_cast<unsigned short>(retVal);
    }

    short ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, short defaultValue) noexcept {
        int retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryIntAttribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return static_cast<short>(std::stoi(values[0]));
            }
            short lower = static_cast<short>(std::stoi(values[0]));
            short upper = static_cast<short>(std::stoi(values[1]));
            retVal = MathUtils::GetRandomIntInRange(lower, upper);
        }
        return static_cast<unsigned short>(retVal);
    }

    unsigned int ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned int defaultValue) noexcept {
        unsigned int retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return static_cast<unsigned short>(std::stoul(values[0]));
            }
            unsigned int lower = static_cast<unsigned int>(std::stoul(values[0]));
            unsigned int upper = static_cast<unsigned int>(std::stoul(values[1]));
            retVal = MathUtils::GetRandomIntInRange(lower, upper);
        }
        return static_cast<unsigned int>(retVal);
    }

    int ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, int defaultValue) noexcept {
        int retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryIntAttribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return std::stoi(values[0]);
            }
            int lower = std::stoi(values[0]);
            int upper = std::stoi(values[1]);
            retVal = MathUtils::GetRandomIntInRange(lower, upper);
        }
        return retVal;
    }

    unsigned long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned long defaultValue) noexcept {
        long long retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryInt64Attribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return std::stoul(values[0]);
            }
            long lower = static_cast<long>(std::stoll(values[0]));
            long upper = static_cast<long>(std::stoll(values[1]));
            retVal = MathUtils::GetRandomLongInRange(lower, upper);
        }
        return static_cast<unsigned long>(retVal);
    }

    long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long defaultValue) noexcept {
        long long retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryInt64Attribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return static_cast<long>(std::stoll(values[0]));
            }
            long lower = static_cast<long>(std::stoll(values[0]));
            long upper = static_cast<long>(std::stoll(values[1]));
            retVal = MathUtils::GetRandomLongInRange(lower, upper);
        }
        return static_cast<long>(retVal);
    }

    unsigned long long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, unsigned long long defaultValue) noexcept {
        unsigned long long retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryUnsigned64Attribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return std::stoull(values[0]);
            }
            unsigned long long lower = std::stoull(values[0]);
            unsigned long long upper = std::stoull(values[1]);
            retVal = MathUtils::GetRandomLongLongInRange(lower, upper);
        }
        return static_cast<unsigned long long>(retVal);
    }

    long long ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long long defaultValue) noexcept {
        long long retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryInt64Attribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return std::stoll(values[0]);
            }
            long long lower = std::stoll(values[0]);
            long long upper = std::stoll(values[1]);
            retVal = MathUtils::GetRandomLongLongInRange(lower, upper);
        }
        return static_cast<long long>(retVal);
    }

    float ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, float defaultValue) noexcept {
        float retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryFloatAttribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return std::stof(values[0]);
            }
            float lower = std::stof(values[0]);
            float upper = std::stof(values[1]);
            retVal = MathUtils::GetRandomFloatInRange(lower, upper);
        }
        return retVal;
    }

    double ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, double defaultValue) noexcept {
        double retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            element.QueryDoubleAttribute(attributeName.c_str(), &retVal);
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return std::stod(values[0]);
            }
            double lower = std::stod(values[0]);
            double upper = std::stod(values[1]);
            retVal = MathUtils::GetRandomDoubleInRange(lower, upper);
        }
        return retVal;
    }

    long double ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, long double defaultValue) noexcept {
        long double retVal = defaultValue;
        auto attrAsCStr = element.Attribute(attributeName.c_str());
        auto attr = std::string(attrAsCStr ? attrAsCStr : "");
        bool is_range = attr.find('~') != std::string::npos;
        if(!is_range) {
            auto valueAsCStr = element.Attribute(attr.c_str());
            std::string value(valueAsCStr ? valueAsCStr : "");
            if(!value.empty()) {
                try {
                    retVal = std::stold(value);
                } catch(...) {
                    return retVal;
                }
            }
        } else {
            auto values = StringUtils::Split(attr, '~');
            if(values.size() == 1) {
                return std::stold(values[0]);
            }
            long double lower = std::stold(values[0]);
            long double upper = std::stold(values[1]);
            retVal = MathUtils::GetRandomLongDoubleInRange(lower, upper);
        }
        return retVal;
    }

    Rgba ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Rgba& defaultValue) noexcept {
        auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return Rgba(textVal);
        }
    }

    Vector2 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector2& defaultValue) noexcept {
        auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return Vector2(textVal);
        }
    }

    IntVector2 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector2& defaultValue) noexcept {
        auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return IntVector2(Vector2(textVal));
        }
    }

    Vector3 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector3& defaultValue) noexcept {
        auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return Vector3(textVal);
        }
    }

    IntVector3 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector3& defaultValue) noexcept {
        auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return IntVector3(Vector3(textVal));
        }
    }

    Vector4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Vector4& defaultValue) noexcept {
        auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return Vector4(textVal);
        }
    }

    IntVector4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const IntVector4& defaultValue) noexcept {
        auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return IntVector4(Vector4(textVal));
        }

    }

    Matrix4 ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const Matrix4& defaultValue) noexcept {
        auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
        std::string textVal(s ? s : "");
        if(textVal.empty()) {
            return defaultValue;
        } else {
            return Matrix4(textVal);
        }
    }


    std::string ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const char* defaultValue) noexcept {
        std::string textVal_default(defaultValue ? defaultValue : "");
        return ParseXmlAttribute(element, attributeName, textVal_default);
    }


    std::string ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const std::string& defaultValue) noexcept {
        auto s = element.Attribute(attributeName.c_str()); //returns nullptr when Attribute not found!
        return (s ? s : defaultValue);
    }

    std::string GetElementName(const XMLElement& elem) noexcept {
        auto* name = elem.Name();
        if(name) {
            return { name };
        }
        return {};
    }

    std::string GetAttributeName(const XMLAttribute& attrib) noexcept {
        auto* name = attrib.Name();
        if(name) {
            return { name };
        }
        return {};
    }

} //end DataUtils
