#pragma once

#include "Engine/Core/Rgba.hpp"

#include "Engine/UI/Element.hpp"

#include <string>

class KerningFont;

namespace UI {

class Canvas;

class Label : public UI::Element {
public:
    explicit Label(UI::Canvas* parent_canvas);
    explicit Label(UI::Canvas* parent_canvas, KerningFont* font, const std::string& text = "Label");
    virtual ~Label() = default;

    virtual void Render(Renderer* renderer) const override;

    const KerningFont* const GetFont() const;
    void SetFont(KerningFont* font);
    void SetText(const std::string& text);
    const std::string& GetText() const;
    std::string& GetText();

    void SetColor(const Rgba& color);
    const Rgba& GetColor() const;
    Rgba& GetColor();

    void SetScale(float value);
    float GetScale() const;
    float GetScale();

    virtual void SetPosition(const Vector4& position) override;
    virtual void SetPositionOffset(const Vector2& offset) override;
    virtual void SetPositionRatio(const Vector2& ratio) override;

protected:
    void CalcBoundsFromFont(KerningFont* font);
private:
    KerningFont* _font = nullptr;
    std::string _text{};
    Rgba _color = Rgba::White;
    float _scale = 1.0f;
};

} //End UI