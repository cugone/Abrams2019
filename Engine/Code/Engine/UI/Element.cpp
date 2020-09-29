#include "Engine/UI/Element.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/UI/Panel.hpp"

#include <sstream>

namespace UI {

NullPanelSlot Element::s_NullPanelSlot = NullPanelSlot{};

Element::Element(Panel* parent /*= nullptr*/) {
    if(parent) {
        _slot = parent->AddChild(this);
    }
}

Element::~Element() {
    RemoveSelf();
}

void Element::RemoveSelf() {
    if(_slot && _slot->parent) {
        _slot->parent->RemoveChild(this);
        _slot->parent = nullptr;
        _slot->content = nullptr;
        _slot = &s_NullPanelSlot;
    }
}

bool Element::HasSlot() const noexcept {
    return _slot != &s_NullPanelSlot;
}

void Element::ResetSlot() noexcept {
    _slot = &s_NullPanelSlot;
}

void Element::SetSlot(PanelSlot* newSlot) noexcept {
    _slot = newSlot;
}

const PanelSlot* const Element::GetSlot() const noexcept {
    return _slot;
}

PanelSlot* Element::GetSlot() noexcept {
    return const_cast<PanelSlot*>(static_cast<const Element&>(*this).GetSlot());
}

void Element::SetBorderColor(const Rgba& color) {
    SetDebugColors(color, _fill_color, _pivot_color);
}

void Element::SetBackgroundColor(const Rgba& color) {
    SetDebugColors(_edge_color, color, _pivot_color);
}

void Element::SetPivotColor(const Rgba& color) {
    SetDebugColors(_edge_color, _fill_color, color);
}

void Element::SetDebugColors(const Rgba& edge, const Rgba& fill, const Rgba& pivot /*= Rgba::RED*/) {
    _edge_color = edge;
    _fill_color = fill;
    _pivot_color = pivot;
}

Vector2 Element::CalcLocalPosition() const {
    AABB2 local_bounds = GetParentBounds();
    return MathUtils::CalcPointFromNormalizedPoint(_position.GetXY(), local_bounds) + _position.GetZW();
}

Vector2 Element::CalcRelativePosition(const Vector2& position) const {
    AABB2 parent_bounds = GetParentLocalBounds();
    return MathUtils::CalcPointFromNormalizedPoint(position, parent_bounds);
}

Vector2 Element::CalcRelativePosition() const {
    AABB2 parent_bounds = GetParentLocalBounds();
    return MathUtils::CalcPointFromNormalizedPoint(_pivot, parent_bounds);
}

const Vector4& Element::GetPosition() const {
    return _position;
}

void Element::SetPosition(const Vector4& position) {
    DirtyElement(InvalidateElementReason::Layout);
    _position = position;
    CalcBounds();
}

void Element::SetPositionRatio(const Vector2& ratio) {
    Element::SetPosition(Vector4{ratio, _position.GetZW()});
}

void Element::SetPositionOffset(const Vector2& offset) {
    Element::SetPosition(Vector4{_position.GetXY(), offset});
}

void Element::SetPivot(const Vector2& pivotPosition) {
    DirtyElement(InvalidateElementReason::Layout);
    _pivot = pivotPosition;
    CalcBounds();
}

void Element::SetPivot(const PivotPosition& pivotPosition) {
    switch(pivotPosition) {
    case PivotPosition::Center:
        SetPivot(Vector2(0.5f, 0.5f));
        break;
    case PivotPosition::TopLeft:
        SetPivot(Vector2(0.0f, 0.0f));
        break;
    case PivotPosition::Top:
        SetPivot(Vector2(0.5f, 0.0f));
        break;
    case PivotPosition::TopRight:
        SetPivot(Vector2(1.0f, 0.0f));
        break;
    case PivotPosition::Right:
        SetPivot(Vector2(1.0f, 0.5f));
        break;
    case PivotPosition::BottomRight:
        SetPivot(Vector2(1.0f, 1.0f));
        break;
    case PivotPosition::Bottom:
        SetPivot(Vector2(0.5f, 1.0f));
        break;
    case PivotPosition::BottomLeft:
        SetPivot(Vector2(0.0f, 1.0f));
        break;
    case PivotPosition::Left:
        SetPivot(Vector2(0.0f, 0.5f));
        break;
    default:
        const auto ss = std::string{__FUNCTION__} + ": Unhandled pivot mode.";
        ERROR_AND_DIE(ss.c_str());
        break;
    }
}

const Vector2& Element::GetPivot() const {
    return _pivot;
}

void Element::Update(TimeUtils::FPSeconds /*deltaSeconds*/) {
    /* DO NOTHING */
}

void Element::Render(Renderer& /*renderer*/) const {
    /* DO NOTHING */
}

void Element::DebugRender(Renderer& renderer) const {
    DebugRenderBoundsAndPivot(renderer);
}

void Element::EndFrame() {
    /* DO NOTHING */
}

Matrix4 Element::GetLocalTransform() const noexcept {
    const auto T = Matrix4::CreateTranslationMatrix(CalcLocalPosition());
    const auto R = Matrix4::Create2DRotationMatrix(CalcLocalRotationRadians());
    const auto S = Matrix4::CreateScaleMatrix(CalcLocalScale());
    const auto M = Matrix4::MakeSRT(S, R, T);
    return M;
}

Vector2 Element::CalcLocalScale() const {
    const auto my_bounds = CalcLocalBounds();
    const auto parent_bounds = GetParentBounds();
    const auto parent_width = parent_bounds.maxs.x - parent_bounds.mins.x;
    const auto my_width = my_bounds.maxs.x - my_bounds.mins.x;
    const auto width_scale = my_width / parent_width;
    const auto parent_height = parent_bounds.maxs.y - parent_bounds.mins.y;
    const auto my_height = my_bounds.maxs.y - my_bounds.mins.y;
    const auto height_scale = my_height / parent_height;
    const auto* parent = GetParent();
    return parent ? Vector2(width_scale, height_scale) : Vector2::ONE;
}

Matrix4 Element::GetWorldTransform() const noexcept {
    return Matrix4::MakeRT(GetLocalTransform(), GetParentWorldTransform());
}

Matrix4 Element::GetParentWorldTransform() const noexcept {
    const auto* parent = GetParent();
    return parent ? parent->GetWorldTransform() : Matrix4::I;
}

void Element::DirtyElement(InvalidateElementReason reason /*= InvalidateElementReason::Any*/) {
    _dirty_reason = reason;
}

void Element::DebugRenderBoundsAndPivot(Renderer& renderer) const {
    DebugRenderBounds(renderer);
    DebugRenderPivot(renderer);
}

void Element::DebugRenderPivot(Renderer& renderer) const {
    const auto world_transform = GetWorldTransform();
    const auto scale = world_transform.GetScale();
    const auto inv_scale_matrix = Matrix4::CalculateInverse(Matrix4::CreateScaleMatrix(Vector3(scale.x * 0.10f, scale.y * 0.10f, 1.0f)));
    const auto pivot_pos = MathUtils::CalcPointFromNormalizedPoint(_pivot, _bounds);
    const auto pivot_pos_matrix = Matrix4::CreateTranslationMatrix(pivot_pos);
    const auto transform = Matrix4::MakeSRT(inv_scale_matrix, world_transform, pivot_pos_matrix);
    renderer.SetMaterial(renderer.GetMaterial("__2D"));
    renderer.SetModelMatrix(transform);
    renderer.DrawX2D(_pivot_color);
}

void Element::DebugRenderBounds(Renderer& renderer) const {
    const auto world_transform = GetWorldTransform();
    renderer.SetModelMatrix(world_transform);
    renderer.SetMaterial(renderer.GetMaterial("__2D"));
    renderer.DrawAABB2(_edge_color, _fill_color);
}

AABB2 Element::GetParentBounds() const noexcept {
    const auto* parent = GetParent();
    return parent ? parent->_bounds : AABB2::ZERO_TO_ONE;
}

Panel* Element::GetParent() const noexcept {
    return _slot->parent;
}

bool Element::IsHidden() const {
    return _hidden;
}

bool Element::IsVisible() const {
    return !_hidden;
}

void Element::Hide() {
    SetHidden();
}

void Element::Show() {
    SetHidden(false);
}

void Element::SetHidden(bool hidden /*= true*/) {
    _hidden = hidden;
}

void Element::ToggleHidden() {
    _hidden = !_hidden;
}

void Element::ToggleVisibility() {
    ToggleHidden();
}

bool Element::IsEnabled() const {
    return _enabled;
}

bool Element::IsDisabled() const {
    return !_enabled;
}

void Element::Enable() {
    _enabled = true;
}

void Element::Disable() {
    _enabled = false;
}

void Element::SetEnabled(bool enabled /*= true*/) {
    _enabled = enabled;
}

void Element::ToggleEnabled() {
    _enabled = !_enabled;
}

const std::string& Element::GetName() const {
    return _name;
}

std::string& Element::GetName() {
    return const_cast<std::string&>(static_cast<const Element&>(*this).GetName());
}

void Element::CalcBounds() noexcept {
    DirtyElement(InvalidateElementReason::Layout);
    const auto desired_size = this->CalcDesiredSize();
    _bounds.mins = desired_size.GetXY();
    _bounds.maxs = desired_size.GetZW();
}

void Element::CalcBoundsAndPivot() noexcept {
    DirtyElement(InvalidateElementReason::Layout);
    const auto slot = GetSlot();
    CalcBounds();
    slot->CalcPivot();
}

AABB2 Element::CalcBoundsRelativeToParent() const noexcept {
    const auto parent = GetParent();
    AABB2 parent_bounds = parent ? parent->CalcLocalBounds() : CalcLocalBounds();
    Vector2 parent_size = parent_bounds.CalcDimensions();

    Vector2 pivot_position = parent_bounds.mins + (parent_size * _position.GetXY() + _position.GetZW());

    AABB2 my_local_bounds = CalcLocalBounds();
    my_local_bounds.Translate(pivot_position);

    return my_local_bounds;
}

AABB2 Element::CalcRelativeBounds() const noexcept {
    Vector2 size = CalcDesiredSize().GetZW();
    Vector2 pivot_position = size * _pivot;

    AABB2 bounds;
    bounds.StretchToIncludePoint(Vector2::ZERO);
    bounds.StretchToIncludePoint(size);
    bounds.Translate(-pivot_position);
    return bounds;
}

AABB2 Element::CalcAbsoluteBounds() const noexcept {
    const auto size = CalcDesiredSize();
    const auto parent_bounds = GetParentBounds();
    const auto pivot_position = MathUtils::CalcPointFromNormalizedPoint(_pivot, parent_bounds);
    AABB2 bounds;
    bounds.StretchToIncludePoint(Vector2::ZERO);
    bounds.StretchToIncludePoint(size.GetZW());
    return CalcAlignedAbsoluteBounds();
}

AABB2 Element::AlignBoundsToContainer(AABB2 bounds, AABB2 container, const Vector2& alignment) const noexcept {
    Vector2 max_distance = MathUtils::CalcPointFromNormalizedPoint(alignment, bounds);
    Vector2 distance = MathUtils::CalcPointFromNormalizedPoint(alignment, container) + max_distance;
    bounds.Translate(distance);
    return bounds;
}

AABB2 Element::CalcAlignedAbsoluteBounds() const noexcept {
    AABB2 parent_bounds = GetParentLocalBounds();
    const auto ratio = _position.GetXY();
    AABB2 alignedBounds = AlignBoundsToContainer(CalcBoundsRelativeToParent(), parent_bounds, ratio);

    const auto unit = _position.GetZW();
    Vector2 normalized_ratio = MathUtils::RangeMap(ratio, Vector2(0.0f, 1.0f), Vector2(-1.0f, 1.0f));
    Vector2 scaled_ratio = normalized_ratio * unit;
    Vector2 offset(scaled_ratio);

    alignedBounds.Translate(offset);

    return alignedBounds;
}

AABB2 Element::CalcLocalBounds() const noexcept {
    return AABB2{CalcDesiredSize()};
}

bool Element::IsDirty(InvalidateElementReason reason /*= InvalidateElementReason::Any*/) const {
    return (_dirty_reason & reason) == reason;
}

bool Element::IsParent() const {
    return !IsChild();
}

bool Element::IsChild() const {
    return GetParent() != nullptr;;
}

AABB2 Element::GetParentLocalBounds() const {
    const auto* parent = GetParent();
    return parent ? parent->CalcLocalBounds() : AABB2(Vector2::ZERO, _bounds.CalcDimensions());
}

AABB2 Element::GetParentRelativeBounds() const {
    const auto* parent = GetParent();
    return parent ? parent->CalcBoundsRelativeToParent() : AABB2{0.0f, 0.0f, 0.0f, 0.0f};
}

AABB2 Element::GetBounds(const AABB2& parent, const Vector4& anchors, const Vector4& offsets) const noexcept {
    Vector2 boundMins = MathUtils::CalcPointFromNormalizedPoint(Vector2(anchors.x, anchors.y), parent) + Vector2(offsets.x, offsets.y);
    Vector2 boundMaxs = MathUtils::CalcPointFromNormalizedPoint(Vector2(anchors.z, anchors.w), parent) + Vector2(offsets.z, offsets.w);
    return AABB2(boundMins, boundMaxs);
}

Vector2 Element::GetSmallestOffset(AABB2 a, AABB2 b) const noexcept {
    const auto width = a.CalcDimensions().x;
    const auto height = a.CalcDimensions().y;
    const auto center = a.CalcCenter();
    b.AddPaddingToSides(-(width * 0.5f), -(height * 0.5f));
    Vector2 closestPoint = MathUtils::CalcClosestPoint(center, b);
    return closestPoint - center;
}

AABB2 Element::MoveToBestFit(const AABB2& obj, const AABB2& container) const noexcept {
    Vector2 offset = GetSmallestOffset(obj, container);
    return obj + offset;
}

float Element::GetAspectRatio() const noexcept {
    const auto dims = _bounds.CalcDimensions();
    return dims.x / dims.y;
}

Vector2 Element::GetTopLeft() const noexcept {
    return _bounds.mins;
}

Vector2 Element::GetTopRight() const noexcept {
    return Vector2{_bounds.maxs.x, _bounds.mins.y};
}

Vector2 Element::GetBottomLeft() const noexcept {
    return Vector2{_bounds.mins.x, _bounds.maxs.y};
}

Vector2 Element::GetBottomRight() const noexcept {
    return _bounds.maxs;
}

bool Element::HasParent() const {
    return GetParent();
}

float Element::GetParentOrientationRadians() const {
    const auto parent = GetParent();
    return parent ? parent->GetOrientationRadians() : 0.0f;
}

float Element::GetParentOrientationDegrees() const {
    const auto parent = GetParent();
    return parent ? parent->GetOrientationDegrees() : 0.0f;
}

void Element::SetOrientationDegrees(float value) {
    _orientationRadians = MathUtils::ConvertDegreesToRadians(value);
}

void Element::SetOrientationRadians(float value) {
    _orientationRadians = value;
}

float Element::GetOrientationDegrees() const {
    return MathUtils::ConvertRadiansToDegrees(GetOrientationRadians());
}

float Element::GetOrientationRadians() const {
    return _orientationRadians;
}

float Element::CalcLocalRotationDegrees() const {
    return MathUtils::ConvertRadiansToDegrees(GetOrientationDegrees());
}

float Element::CalcLocalRotationRadians() const {
    return GetOrientationRadians();
}

float Element::CalcWorldRotationRadians() const {
    return GetParentOrientationRadians() + GetOrientationRadians();
}

float Element::CalcWorldRotationDegrees() const {
    return GetParentOrientationDegrees() + GetOrientationDegrees();
}

float Element::GetInvAspectRatio() const noexcept {
    return 1.0f / GetAspectRatio();
}

} // namespace UI