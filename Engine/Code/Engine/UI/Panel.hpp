#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/UI/Element.hpp"
#include "Engine/UI/PanelSlot.hpp"
#include "Engine/UI/Widget.hpp"

#include <memory>
#include <vector>

class Renderer;

namespace UI {

class Panel : public Element {
public:
    explicit Panel(Widget* owner);
    Panel(Panel&& other) = default;
    Panel& operator=(Panel&& rhs) = default;
    Panel(const Panel& other) = delete;
    Panel& operator=(Panel& other) = delete;
    virtual ~Panel() = default;

    void Update(TimeUtils::FPSeconds deltaSeconds) override;
    void Render(Renderer& renderer) const override;
    void DebugRender(Renderer& renderer) const override;
    void EndFrame() override;
    virtual PanelSlot* AddChild(Element* child)=0;
    virtual PanelSlot* AddChildAt(Element* child, std::size_t index) = 0;
    virtual PanelSlot* AddChildFromXml(const XMLElement& elem, Element* child) = 0;
    virtual PanelSlot* AddChildFromXml(const XMLElement& elem, Element* child, std::size_t index) = 0;
    virtual void RemoveChild(Element* child) = 0;
    virtual void RemoveAllChildren() = 0;

    [[nodiscard]] const Widget* const GetOwningWidget() const noexcept;
    void SetOwningWidget(Widget* owner) noexcept;

    [[nodiscard]] Vector4 CalcDesiredSize() const noexcept override;

    void DebugRenderBottomUp(Renderer& renderer) const;
    void DebugRenderTopDown(Renderer& renderer) const;
    void DebugRenderChildren(Renderer& renderer) const;
protected:

    [[nodiscard]] virtual AABB2 CalcChildrenDesiredBounds() const = 0;
    virtual void ArrangeChildren() noexcept = 0;
    [[nodiscard]] virtual bool LoadFromXml(const XMLElement& elem) noexcept = 0;
    virtual void UpdateChildren(TimeUtils::FPSeconds);
    virtual void RenderChildren(Renderer&) const;
    virtual void SortChildren();

    void CalcBoundsForChildren() noexcept;
    void CalcBoundsForMeThenMyChildren() noexcept;
    void CalcBoundsMyChildrenThenMe() noexcept;

    [[nodiscard]] virtual bool CanHaveManyChildren() const noexcept;
    std::vector<std::shared_ptr<PanelSlot>> _slots{};

private:
    Widget* _owner{};
};

} // namespace UI