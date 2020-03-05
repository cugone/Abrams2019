#include "Engine/UI/Panel.hpp"

#include "Engine/UI/Canvas.hpp"

namespace UI {

Panel::Panel(UI::Canvas* parent_canvas)
: Element(parent_canvas) {
    /* DO NOTHING */
}

void Panel::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(IsDisabled()) {
        return;
    }
    UpdateChildren(deltaSeconds);
}

void Panel::Render(Renderer& renderer) const {
    if(IsHidden()) {
        return;
    }
    if(0 < _edge_color.a || 0 < _fill_color.a) {
        DebugRenderBounds(renderer);
    }
    RenderChildren(renderer);
}

void Panel::DebugRender(Renderer& renderer, bool showSortOrder /*= false*/) const {
    DebugRenderBottomUp(renderer, showSortOrder);
}

} // namespace UI