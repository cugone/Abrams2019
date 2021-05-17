#include "Engine/UI/Panel.hpp"

#include "Engine/UI/Canvas.hpp"

namespace UI {

Panel::Panel(Widget* owner)
: _owner(owner) {
    /* DO NOTHING */
}

void Panel::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(IsDisabled()) {
        return;
    }
    UpdateChildren(deltaSeconds);
}

//void Panel::Update(TimeUtils::FPSeconds deltaSeconds) {
//    if(IsDisabled()) {
//        return;
//    }
//    UpdateChildren(deltaSeconds);
//}

void Panel::Render(Renderer& renderer) const {
    if(IsHidden()) {
        return;
    }
    if(0 < _edge_color.a || 0 < _fill_color.a) {
        DebugRenderBounds(renderer);
    }
    RenderChildren(renderer);
}

void Panel::DebugRender(Renderer& renderer) const {
    DebugRenderBottomUp(renderer);
}

void Panel::EndFrame() {
    CalcBoundsForMeThenMyChildren();
}

const Widget* const Panel::GetOwningWidget() const noexcept {
    return _owner;
}

void Panel::SetOwningWidget(Widget* owner) noexcept {
    _owner = owner;
}

Vector4 Panel::CalcDesiredSize() const noexcept {
    return Vector4::ZERO;
}

void Panel::DebugRenderBottomUp(Renderer& renderer) const {
    DebugRenderBoundsAndPivot(renderer);
    DebugRenderChildren(renderer);
}

void Panel::DebugRenderTopDown(Renderer& renderer) const {
    DebugRenderChildren(renderer);
    DebugRenderBoundsAndPivot(renderer);
}

void Panel::DebugRenderChildren(Renderer& renderer) const {
    for(auto& slot : _slots) {
        if(slot) {
            slot->content->DebugRender(renderer);
        }
    }
}

void Panel::SortChildren() {
}

void Panel::CalcBoundsForChildren() noexcept {
    for(auto& slot : _slots) {
        if(slot && slot->content) {
            slot->content->CalcBounds();
        }
    }
}

void Panel::CalcBoundsForMeThenMyChildren() noexcept {
    CalcBounds();
    CalcBoundsForChildren();
}

void Panel::CalcBoundsMyChildrenThenMe() noexcept {
    CalcBoundsForChildren();
    CalcBounds();
}

bool Panel::CanHaveManyChildren() const noexcept {
    return true;
}

void Panel::UpdateChildren(TimeUtils::FPSeconds) {
}

void Panel::RenderChildren(Renderer&) const {
}

} // namespace UI