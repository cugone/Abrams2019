#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/UI/Element.hpp"

#include <string>

class KerningFont;

namespace UI {

class Canvas;

class Label : public Element {
public:
    explicit Label(Panel* parent);
    explicit Label(const XMLElement& elem, Panel* parent = nullptr);
    explicit Label(Panel* parent, KerningFont* font, const std::string& text = "Label");
    virtual ~Label() = default;

    virtual void Render(Renderer& renderer) const override;

    [[nodiscard]] const KerningFont* const GetFont() const;
    void SetFont(KerningFont* font);
    void SetText(const std::string& text);
    [[nodiscard]] const std::string& GetText() const;
    [[nodiscard]] std::string& GetText();

    void SetColor(const Rgba& color);
    [[nodiscard]] const Rgba& GetColor() const;
    [[nodiscard]] Rgba& GetColor();

    void SetScale(float value);
    [[nodiscard]] float GetScale() const;
    [[nodiscard]] float GetScale();

    virtual void SetPosition(const Vector4& position) override;
    virtual void SetPositionOffset(const Vector2& offset) override;
    virtual void SetPositionRatio(const Vector2& ratio) override;

    [[nodiscard]] Vector4 CalcDesiredSize() const noexcept override;

protected:
    [[nodiscard]] Vector2 CalcBoundsFromFont(KerningFont* font) const;

private:
    [[nodiscard]] bool LoadFromXml(const XMLElement& elem) noexcept;

    KerningFont* _font = nullptr;
    std::string _fontname{"System32"};
    std::string _text{};
    Rgba _color = Rgba::White;
    float _scale = 1.0f;
};

} // namespace UI