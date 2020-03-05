#include "Engine/UI/Sprite.hpp"

#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace UI {

Sprite::Sprite(Canvas* parent_canvas, AnimatedSprite* sprite)
: Element(parent_canvas)
, _sprite(sprite) {
    auto dims = _sprite->GetFrameDimensions();
    auto w = static_cast<float>(dims.x);
    auto h = static_cast<float>(dims.y);
    SetSize(UI::Metric{UI::Ratio{}, Vector2(w, h)});
}

Sprite::~Sprite() {
    delete _sprite;
    _sprite = nullptr;
}

void Sprite::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(IsDisabled()) {
        return;
    }
    _sprite->Update(deltaSeconds);
}

void Sprite::Render(Renderer& renderer) const {
    if(IsHidden()) {
        return;
    }
    renderer.SetModelMatrix(GetWorldTransform());
    renderer.SetMaterial(_sprite->GetMaterial());
    auto cur_tc = _sprite->GetCurrentTexCoords();
    Vector4 tex_coords(cur_tc.mins, cur_tc.maxs);
    renderer.DrawQuad2D(tex_coords);
}

void Sprite::DebugRender(Renderer& renderer, bool showSortOrder /*= false*/) const {
    Element::DebugRender(renderer, showSortOrder);
}

} // namespace UI