#include "Engine/UI/PictureBox.hpp"

#include "Engine/Core/FileUtils.hpp"

#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Engine/UI/Panel.hpp"
#include "Engine/UI/Widget.hpp"

namespace UI {

PictureBox::PictureBox(Panel* parent /*= nullptr*/)
: Element(parent)
{
    /* DO NOTHING */
}
PictureBox::PictureBox(const XMLElement& elem, Panel* parent /*= nullptr*/)
: Element(parent)
{
    LoadFromXml(elem);
}

void PictureBox::SetImage(std::unique_ptr<AnimatedSprite> sprite) noexcept {
    _sprite.reset(sprite.release());
    auto dims = _sprite->GetFrameDimensions();
    GetSlot()->CalcPivot();
}

const AnimatedSprite* const PictureBox::GetImage() const noexcept {
    return _sprite.get();
}

void PictureBox::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(IsDisabled()) {
        return;
    }
    _sprite->Update(deltaSeconds);
}

void PictureBox::Render(Renderer& renderer) const {
    if(IsHidden()) {
        return;
    }
    renderer.SetModelMatrix(GetWorldTransform());
    renderer.SetMaterial(_sprite->GetMaterial());
    auto cur_tc = _sprite->GetCurrentTexCoords();
    Vector4 tex_coords(cur_tc.mins, cur_tc.maxs);
    renderer.DrawQuad2D(tex_coords);
}

void PictureBox::DebugRender(Renderer& renderer) const {
    Element::DebugRender(renderer);
}


Vector4 PictureBox::CalcDesiredSize() const noexcept {
    auto dims = _sprite->GetFrameDimensions();
    auto w = static_cast<float>(dims.x);
    auto h = static_cast<float>(dims.y);
    return Vector4{Vector2::ZERO, Vector2{w, h}};
}

bool PictureBox::LoadFromXml(const XMLElement& elem) noexcept {
    DataUtils::ValidateXmlElement(elem, "picturebox", "", "name,src", "");
    _name = DataUtils::ParseXmlAttribute(elem, "name", _name);
    const auto src = DataUtils::ParseXmlAttribute(elem, "src", "");
    FileUtils::IsSafeReadPath(src);
    if(const auto* parent = GetParent()) {
        _sprite = parent->GetOwningWidget()->GetRenderer().CreateAnimatedSprite(src);
    }
    return true;
}


} // namespace UI