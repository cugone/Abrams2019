#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/UI/Types.hpp"
#include "Engine/UI/PanelSlot.hpp"

class Renderer;

namespace UI {

class Panel;
struct PanelSlot;

class Element {
public:
    explicit Element(Panel* parent = nullptr);
    virtual ~Element() = 0;

    virtual void Update(TimeUtils::FPSeconds deltaSeconds);
    virtual void Render(Renderer& renderer) const;
    virtual void DebugRender(Renderer& renderer) const;
    virtual void EndFrame();
    void SetBorderColor(const Rgba& color);
    void SetBackgroundColor(const Rgba& color);
    void SetPivotColor(const Rgba& color);
    void SetDebugColors(const Rgba& edge, const Rgba& fill, const Rgba& pivot = Rgba::Red);

    const Vector4& GetPosition() const;
    virtual void SetPosition(const Vector4& position);
    virtual void SetPositionOffset(const Vector2& offset);
    virtual void SetPositionRatio(const Vector2& ratio);

    virtual Vector4 CalcDesiredSize() const noexcept = 0;

    void SetPivot(const Vector2& pivotPosition);
    const Vector2& GetPivot() const;
    void SetPivot(const PivotPosition& pivotPosition);

    void SetOrientationDegrees(float value);
    void SetOrientationRadians(float value);
    float GetOrientationDegrees() const;
    float GetOrientationRadians() const;

    bool HasParent() const;
    AABB2 GetParentBounds() const noexcept;

    Panel* GetParent() const noexcept;

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

    const std::string& GetName() const;
    std::string& GetName();

    void RemoveSelf();

    bool HasSlot() const noexcept;
    void ResetSlot() noexcept;
    void SetSlot(PanelSlot* newSlot) noexcept;
    const PanelSlot* const GetSlot() const noexcept;
    PanelSlot* GetSlot() noexcept;

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
    void CalcBoundsAndPivot() noexcept;
    AABB2 CalcBoundsRelativeToParent() const noexcept;

    AABB2 AlignBoundsToContainer(AABB2 bounds, AABB2 container, const Vector2& alignment) const noexcept;
    AABB2 CalcRelativeBounds() const noexcept;
    AABB2 CalcAbsoluteBounds() const noexcept;
    AABB2 CalcAlignedAbsoluteBounds() const noexcept;
    AABB2 CalcLocalBounds() const noexcept;

    Matrix4 GetLocalTransform() const noexcept;
    Matrix4 GetWorldTransform() const noexcept;
    Matrix4 GetParentWorldTransform() const noexcept;

    void DirtyElement(InvalidateElementReason reason = InvalidateElementReason::Any);
    bool IsDirty(InvalidateElementReason reason = InvalidateElementReason::Any) const;
    bool IsParent() const;
    bool IsChild() const;

    void DebugRenderBoundsAndPivot(Renderer& renderer) const;
    void DebugRenderPivot(Renderer& renderer) const;
    void DebugRenderBounds(Renderer& renderer) const;

    AABB2 GetParentLocalBounds() const;
    AABB2 GetParentRelativeBounds() const;

    AABB2 GetBounds(const AABB2& parent, const Vector4& anchors, const Vector4& offsets) const noexcept;
    Vector2 GetSmallestOffset(AABB2 a, AABB2 b) const noexcept;
    AABB2 MoveToBestFit(const AABB2& obj, const AABB2& container) const noexcept;

    float GetAspectRatio() const noexcept;
    float GetInvAspectRatio() const noexcept;

    Vector2 GetTopLeft() const noexcept;
    Vector2 GetTopRight() const noexcept;
    Vector2 GetBottomLeft() const noexcept;
    Vector2 GetBottomRight() const noexcept;

    std::string _name{};
    Rgba _fill_color = Rgba::NoAlpha;
    Rgba _edge_color = Rgba::White;

private:
    Vector4 _position{};
    Vector2 _pivot{};
    AABB2 _bounds{};
    Rgba _pivot_color = Rgba::Red;
    PanelSlot* _slot = &s_NullPanelSlot;
    float _orientationRadians = 0.0f;
    InvalidateElementReason _dirty_reason = InvalidateElementReason::None;
    bool _hidden = false;
    bool _enabled = true;

    float GetParentOrientationRadians() const;
    float GetParentOrientationDegrees() const;

    friend class Panel;
    friend struct PanelSlot;
    static NullPanelSlot s_NullPanelSlot;
};

} // namespace UI