#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/UI/Types.hpp"

class Renderer;

namespace UI {

class Canvas;

class Element {
public:
    Element() = default;
    explicit Element(UI::Canvas* parent_canvas);
    virtual ~Element() = 0;

    virtual void Update(TimeUtils::FPSeconds deltaSeconds);
    virtual void Render(Renderer& renderer) const;
    virtual void DebugRender(Renderer& renderer, bool showSortOrder = false) const;

    template<typename T>
    T* CreateChild();
    template<typename T, typename... Args>
    T* CreateChild(Args&&... args);
    template<typename T>
    T* CreateChild(UI::Canvas* parentCanvas);
    template<typename T, typename... Args>
    T* CreateChild(UI::Canvas* parentCanvas, Args&&... args);
    template<typename T>
    T* CreateChildBefore(UI::Element* youngerSibling);
    template<typename T, typename... Args>
    T* CreateChildBefore(UI::Element* youngerSibling, Args&&... args);
    template<typename T>
    T* CreateChildBefore(UI::Canvas* parentCanvas, UI::Element* youngerSibling);
    template<typename T, typename... Args>
    T* CreateChildBefore(UI::Canvas* parentCanvas, UI::Element* youngerSibling, Args&&... args);
    template<typename T>
    T* CreateChildAfter(UI::Element* olderSibling);
    template<typename T, typename... Args>
    T* CreateChildAfter(UI::Element* olderSibling, Args&&... args);
    template<typename T>
    T* CreateChildAfter(UI::Canvas* parentCanvas, UI::Element* olderSibling);
    template<typename T, typename... Args>
    T* CreateChildAfter(UI::Canvas* parentCanvas, UI::Element* olderSibling, Args&&... args);

    UI::Element* AddChild(UI::Element* child);
    UI::Element* AddChildBefore(UI::Element* child, UI::Element* younger_sibling);
    UI::Element* AddChildAfter(UI::Element* child, UI::Element* older_sibling);

    void RemoveChild(Element* child);
    void RemoveAllChildren();
    void RemoveSelf();

    void DestroyChild(UI::Element*& child);
    void DestroyAllChildren();

    void SetBorderColor(const Rgba& color);
    void SetBackgroundColor(const Rgba& color);
    void SetPivotColor(const Rgba& color);
    void SetDebugColors(const Rgba& edge, const Rgba& fill, const Rgba& pivot = Rgba::Red);

    void SetSize(const Metric& size);
    Vector2 GetSize() const noexcept;

    const Vector4& GetPosition() const;
    virtual void SetPosition(const Vector4& position);
    virtual void SetPositionOffset(const Vector2& offset);
    virtual void SetPositionRatio(const Vector2& ratio);

    void SetPivot(const Vector2& pivotPosition);
    const Vector2& GetPivot() const;
    void SetPivot(const PivotPosition& pivotPosition);

    void SetOrientationDegrees(float value);
    void SetOrientationRadians(float value);
    float GetOrientationDegrees() const;
    float GetOrientationRadians() const;

    void SetOrder(std::size_t value);
    std::size_t GetOrder() const;

    bool HasParent() const;
    AABB2 GetParentBounds() const noexcept;

    bool IsHidden() const;
    bool IsVisible() const;
    void Hide();
    void Show();
    void SetHidden(bool hidden = true);
    void ToggleHidden();
    void ToggleVisibility();

    bool IsEnabled() const;
    bool IsDisabled() const;
    void Enable();
    void Disable();
    void SetEnabled(bool enabled = true);
    void ToggleEnabled();

protected:
    Vector2 CalcLocalPosition() const;
    Vector2 CalcLocalScale() const;
    float CalcLocalRotationRadians() const;
    float CalcLocalRotationDegrees() const;
    float CalcWorldRotationRadians() const;
    float CalcWorldRotationDegrees() const;

    Vector2 CalcRelativePosition() const;
    Vector2 CalcRelativePosition(const Vector2& position) const;

    void CalcBounds() noexcept;
    AABB2 CalcBoundsRelativeToParent() const noexcept;

    void CalcBoundsForChildren() noexcept;
    void CalcBoundsForMeThenMyChildren() noexcept;
    AABB2 AlignBoundsToContainer(AABB2 bounds, AABB2 container, const Vector2& alignment) const noexcept;
    AABB2 CalcRelativeBounds() const noexcept;
    AABB2 CalcAbsoluteBounds() const noexcept;
    AABB2 CalcAlignedAbsoluteBounds() const noexcept;
    AABB2 CalcLocalBounds() const noexcept;

    Matrix4 GetLocalTransform() const noexcept;
    Matrix4 GetWorldTransform() const noexcept;
    Matrix4 GetParentWorldTransform() const noexcept;

    void DirtyElement();
    bool IsDirty() const;
    bool IsParent() const;
    bool IsChild() const;

    UI::Canvas* GetParentCanvas() const;
    void SetParentCanvas(UI::Canvas* canvas);

    void ReorderAllChildren();

    void DebugRenderBottomUp(Renderer& renderer, bool showSortOrder = false) const;
    void DebugRenderTopDown(Renderer& renderer, bool showSortOrder = false) const;
    void DebugRenderChildren(Renderer& renderer, bool showSortOrder = false) const;
    void DebugRenderBoundsAndPivot(Renderer& renderer) const;
    void DebugRenderPivot(Renderer& renderer) const;
    void DebugRenderBounds(Renderer& renderer) const;
    void DebugRenderOrder(Renderer& renderer) const;
    AABB2 GetParentLocalBounds() const;
    AABB2 GetParentRelativeBounds() const;
    void UpdateChildren(TimeUtils::FPSeconds deltaSeconds);
    void RenderChildren(Renderer& renderer) const;

    AABB2 GetBounds(const AABB2& parent, const Vector4& anchors, const Vector4& offsets) const noexcept;
    Vector2 GetSmallestOffset(AABB2 a, AABB2 b) const noexcept;
    AABB2 MoveToBestFit(const AABB2& obj, const AABB2& container) const noexcept;

    float GetAspectRatio() const noexcept;
    float GetInvAspectRatio() const noexcept;

    Vector2 GetTopLeft() const noexcept;
    Vector2 GetTopRight() const noexcept;
    Vector2 GetBottomLeft() const noexcept;
    Vector2 GetBottomRight() const noexcept;

    Metric _size{};
    Rgba _fill_color = Rgba::NoAlpha;
    Rgba _edge_color = Rgba::White;

private:
    Vector4 _position{};
    Vector2 _pivot{};
    PositionMode _mode{};
    Rgba _pivot_color = Rgba::Red;
    Element* _parent = nullptr;
    std::vector<Element*> _children{};
    UI::Canvas* _parent_canvas = nullptr;
    AABB2 _bounds{};
    float _orientationRadians = 0.0f;
    std::size_t _order = 0;
    bool _dirty_bounds = false;
    bool _hidden = false;
    bool _enabled = true;

    float GetParentOrientationRadians() const;
    float GetParentOrientationDegrees() const;
    void SortChildren();
    void SortAllChildren();
};

template<typename T>
T* UI::Element::CreateChild() {
    return dynamic_cast<T*>(AddChild(new T{}));
}

template<typename T, typename... Args>
T* UI::Element::CreateChild(Args&&... args) {
    return dynamic_cast<T*>(AddChild(new T{std::forward<Args>(args)...}));
}
template<typename T>
T* UI::Element::CreateChild(UI::Canvas* parentCanvas) {
    return dynamic_cast<T*>(AddChild(new T{parentCanvas}));
}

template<typename T, typename... Args>
T* UI::Element::CreateChild(UI::Canvas* parentCanvas, Args&&... args) {
    return dynamic_cast<T*>(AddChild(new T{parentCanvas, std::forward<Args>(args)...}));
}

template<typename T>
T* UI::Element::CreateChildBefore(UI::Element* youngerSibling) {
    return dynamic_cast<T*>(AddChildBefore(new T{}, youngerSibling));
}

template<typename T>
T* UI::Element::CreateChildBefore(UI::Canvas* parentCanvas, UI::Element* youngerSibling) {
    return dynamic_cast<T*>(AddChildBefore(new T{parentCanvas}, youngerSibling));
}
template<typename T, typename... Args>
T* UI::Element::CreateChildBefore(UI::Element* youngerSibling, Args&&... args) {
    return dynamic_cast<T*>(AddChildBefore(new T{std::forward<Args>(args)...}, youngerSibling));
}
template<typename T, typename... Args>
T* UI::Element::CreateChildBefore(UI::Canvas* parentCanvas, UI::Element* youngerSibling, Args&&... args) {
    return dynamic_cast<T*>(AddChildBefore(new T{parentCanvas, std::forward<Args>(args)...}, youngerSibling));
}
template<typename T>
T* UI::Element::CreateChildAfter(UI::Element* olderSibling) {
    return dynamic_cast<T*>(AddChildAfter(new T{}, olderSibling));
}
template<typename T, typename... Args>
T* UI::Element::CreateChildAfter(UI::Element* olderSibling, Args&&... args) {
    return dynamic_cast<T*>(AddChildAfter(new T{std::forward<Args>(args)...}, olderSibling));
}

template<typename T>
T* UI::Element::CreateChildAfter(UI::Canvas* parentCanvas, UI::Element* olderSibling) {
    return dynamic_cast<T*>(AddChildAfter(new T{parentCanvas}, olderSibling));
}

template<typename T, typename... Args>
T* UI::Element::CreateChildAfter(UI::Canvas* parentCanvas, UI::Element* olderSibling, Args&&... args) {
    return dynamic_cast<T*>(AddChildAfter(new T{parentCanvas, std::forward<Args>(args)...}, olderSibling));
}

} // namespace UI