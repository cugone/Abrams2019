#pragma once

#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector3.hpp"

#include <ostream>
#include <string>
#include <vector>

class Rgba {
public:

    static const Rgba White;
    static const Rgba Black;
    static const Rgba Red;
    static const Rgba Pink;
    static const Rgba Green;
    static const Rgba ForestGreen;
    static const Rgba Blue;
    static const Rgba NavyBlue;
    static const Rgba Cyan;
    static const Rgba Yellow;
    static const Rgba Magenta;
    static const Rgba Orange;
    static const Rgba Violet;
    static const Rgba LightGrey;
    static const Rgba LightGray;
    static const Rgba Grey;
    static const Rgba Gray;
    static const Rgba DarkGrey;
    static const Rgba DarkGray;
    static const Rgba Olive;
    static const Rgba SkyBlue;
    static const Rgba Lime;
    static const Rgba Teal;
    static const Rgba Turquoise;
    static const Rgba Periwinkle;
    static const Rgba NormalZ;
    static const Rgba NoAlpha;

    static Rgba Random();
    static Rgba RandomGreyscale();
    static Rgba RandomGrayscale();
    static Rgba RandomWithAlpha();
    static Rgba RandomLessThan(const Rgba& color);

    Rgba() = default;
    Rgba(const Rgba& rhs) = default;
    Rgba(Rgba&& rhs) = default;
    Rgba& operator=(const Rgba& rhs) = default;
    Rgba& operator=(Rgba&& rhs) = default;
    ~Rgba() = default;

    explicit Rgba(std::string name);
    explicit Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0xFF);

    void SetAsBytes(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
    void SetAsFloats(float normalized_red, float normalized_green, float normalized_blue, float normalized_alpha);
    void GetAsFloats(float& out_normalized_red, float& out_normalized_green, float& out_normalized_blue, float& out_normalized_alpha) const;
    Vector4 GetRgbaAsFloats() const;
    Vector3 GetRgbAsFloats() const;
    void ScaleRGB(float scale);
    void ScaleAlpha(float scale);

    uint32_t GetAsRawValue() const;
    void SetFromRawValue(uint32_t value);
    void SetRGBAFromRawValue(uint32_t value);
    void SetRGBFromRawValue(uint32_t value);
    void SetRgbFromFloats(const Vector3& value);
    void SetRgbaFromFloats(const Vector4& value);
    bool IsRgbEqual(const Rgba& rhs) const;
    bool operator==(const Rgba& rhs) const;
    bool operator!=(const Rgba& rhs) const;
    bool operator<(const Rgba& rhs) const;

    Rgba& operator+=(const Rgba& rhs);
    friend Rgba operator+(Rgba lhs, const Rgba& rhs);

    Rgba& operator-=(const Rgba& rhs);
    friend Rgba operator-(Rgba lhs, const Rgba& rhs);

    Rgba& operator++();
    Rgba operator++(int);

    Rgba& operator--();
    Rgba operator--(int);

    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;
    unsigned char a = 255;

    friend std::ostream& operator<<(std::ostream& os, const Rgba& rhs);

protected:
private:
    void SetValueFromName(std::string name);
};