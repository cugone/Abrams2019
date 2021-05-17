#include "Engine/UI/UIPanel.hpp"

#include "Engine/UI/UICanvas.hpp"

UIPanel::UIPanel(UIWidget* owner)
: _owner(owner) {
    /* DO NOTHING */
}

void UIPanel::Update(TimeUtils::FPSeconds deltaSeconds) {
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

void UIPanel::Render(Renderer& renderer) const {
    if(IsHidden()) {
        return;
    }
    if(0 < _edge_color.a || 0 < _fill_color.a) {
        DebugRenderBounds(renderer);
    }
    RenderChildren(renderer);
}

void UIPanel::DebugRender(Renderer& renderer) const {
    DebugRenderBottomUp(renderer);
}

void UIPanel::EndFrame() {
    CalcBoundsForMeThenMyChildren();
}

const UIWidget* const UIPanel::GetOwningWidget() const noexcept {
    return _owner;
}

void UIPanel::SetOwningWidget(UIWidget* owner) noexcept {
    _owner = owner;
}

Vector4 UIPanel::CalcDesiredSize() const noexcept {
    return Vector4::ZERO;
}

void UIPanel::DebugRenderBottomUp(Renderer& renderer) const {
    DebugRenderBoundsAndPivot(renderer);
    DebugRenderChildren(renderer);
}

void UIPanel::DebugRenderTopDown(Renderer& renderer) const {
    DebugRenderChildren(renderer);
    DebugRenderBoundsAndPivot(renderer);
}

void UIPanel::DebugRenderChildren(Renderer& renderer) const {
    for(auto& slot : _slots) {
        if(slot) {
            slot->content->DebugRender(renderer);
        }
    }
}

void UIPanel::SortChildren() {
}

void UIPanel::CalcBoundsForChildren() noexcept {
    for(auto& slot : _slots) {
        if(slot && slot->content) {
            slot->content->CalcBounds();
        }
    }
}

void UIPanel::CalcBoundsForMeThenMyChildren() noexcept {
    CalcBounds();
    CalcBoundsForChildren();
}

void UIPanel::CalcBoundsMyChildrenThenMe() noexcept {
    CalcBoundsForChildren();
    CalcBounds();
}

bool UIPanel::CanHaveManyChildren() const noexcept {
    return true;
}

void UIPanel::UpdateChildren(TimeUtils::FPSeconds) {
}

void UIPanel::RenderChildren(Renderer&) const {
}
