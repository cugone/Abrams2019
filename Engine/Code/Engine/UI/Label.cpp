#include "Engine/UI/Label.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/UI/Panel.hpp"
#include "Engine/UI/Canvas.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/UI/Types.hpp"

namespace UI {

Label::Label(Panel* parent)
: Element(parent) {
    /* DO NOTHING */
}

Label::Label(Panel* parent, KerningFont* font, const std::string& text /*= "Label"*/)
: Element(parent)
, _font(font)
, _text(text) {
    const auto desired_size = CalcDesiredSize();
    _bounds.mins = desired_size.GetXY();
    _bounds.maxs = desired_size.GetZW();
}

Label::Label(const XMLElement& elem, Panel* parent /*= nullptr*/)
: Element(parent)
{
    GUARANTEE_OR_DIE(LoadFromXml(elem), "Label constructor failed to load.");
}

void Label::Render(Renderer& renderer) const {
    if(IsHidden()) {
        return;
    }
    const auto world_transform = GetWorldTransform();
    const auto inv_scale = 1.0f / world_transform.GetScale();
    const auto inv_scale_matrix = Matrix4::CreateScaleMatrix(inv_scale);
    const auto model = Matrix4::MakeRT(inv_scale_matrix, world_transform);
    renderer.SetModelMatrix(model);
    renderer.SetMaterial(_font->GetMaterial());
    renderer.DrawMultilineText(_font, _text, _color);
}

const KerningFont* const Label::GetFont() const {
    return _font;
}

void Label::SetFont(KerningFont* font) {
    _font = font;
    DirtyElement();
    CalcBounds();
}

void Label::SetText(const std::string& text) {
    _text = text;
    DirtyElement();
    CalcBounds();
}

const std::string& Label::GetText() const {
    return _text;
}

std::string& Label::GetText() {
    return _text;
}

void Label::SetColor(const Rgba& color) {
    _color = color;
}

const Rgba& Label::GetColor() const {
    return _color;
}

Rgba& Label::GetColor() {
    return _color;
}

void Label::SetScale(float value) {
    _scale = value;
    DirtyElement();
    CalcBounds();
}

float Label::GetScale() const {
    return _scale;
}

float Label::GetScale() {
    return static_cast<const Label&>(*this).GetScale();
}

void Label::SetPosition(const Vector4& position) {
    Element::SetPosition(position);
}

void Label::SetPositionOffset(const Vector2& offset) {
    Element::SetPositionOffset(offset);
}

void Label::SetPositionRatio(const Vector2& ratio) {
    Element::SetPositionRatio(ratio);
}

Vector4 Label::CalcDesiredSize() const noexcept {
    const auto desired_size = CalcBoundsFromFont(_font);
    return Vector4{Vector2::ZERO, desired_size};
}

Vector2 Label::CalcBoundsFromFont(KerningFont* font) const {
    if(font == nullptr) {
        return {};
    }
    const float width = font->CalculateTextWidth(_text, _scale);
    const float height = font->CalculateTextHeight(_text, _scale);
    return Vector2{width, height};
}

bool Label::LoadFromXml(const XMLElement& elem) noexcept {
    DataUtils::ValidateXmlElement(elem, "label", "", "name", "canvas,label,panel,picturebox,button,slot", "font,value");
    _name = DataUtils::ParseXmlAttribute(elem, "name", _name);
    _fontname = DataUtils::ParseXmlAttribute(elem, "font", _fontname);
    _font = GetParent()->GetOwningWidget()->GetRenderer().GetFont(_fontname);
    _text = DataUtils::ParseXmlAttribute(elem, "value", "TEXT");

    if(auto* xml_slot = elem.FirstChildElement("slot")) {
        auto* parent = GetParent();
        parent->RemoveChild(this);
        parent->AddChildFromXml(*xml_slot, this);
    }
    return true;
}

} // namespace UI