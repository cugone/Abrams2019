#include "Engine/UI/Element.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/KerningFont.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/UI/Canvas.hpp"

#include <sstream>

namespace UI {

Element::Element(UI::Canvas* parent_canvas)
    : _parent_canvas(parent_canvas)
{
    /* DO NOTHING */
}

Element::~Element() {
    RemoveSelf();
    DestroyAllChildren();
}

Element* Element::AddChild(Element* child) {
    _dirty_bounds = true;
    _children.push_back(child);
    child->_parent = this;
    child->_order = _children.size();
    CalcBoundsForMeThenMyChildren();
    ReorderAllChildren();
    return child;
}

UI::Element* Element::AddChildBefore(UI::Element* child, UI::Element* younger_sibling) {
    auto result = AddChild(child);
    if(younger_sibling->_order > 0) {
        result->_order = younger_sibling->_order - static_cast<std::size_t>(1);
    } else {
        result->_order = younger_sibling->_order;
        ++(younger_sibling->_order);
    }
    ReorderAllChildren();
    CalcBoundsForMeThenMyChildren();
    return child;
}

UI::Element* Element::AddChildAfter(UI::Element* child, UI::Element* older_sibling) {
    auto result = AddChild(child);
    if(older_sibling->_order < _children.size() - 1) {
        result->_order = older_sibling->_order + static_cast<std::size_t>(1);
    } else {
        result->_order = older_sibling->_order;
        --(older_sibling->_order);
    }
    ReorderAllChildren();
    CalcBoundsForMeThenMyChildren();
    return child;
}

void Element::RemoveChild(Element* child) {
    _dirty_bounds = true;
    _children.erase(
        std::remove_if(_children.begin(), _children.end(),
            [&child](UI::Element* c) {
        return child == c;
    }),
        _children.end());
    ReorderAllChildren();
    CalcBoundsForMeThenMyChildren();
}

void Element::RemoveAllChildren() {
    _dirty_bounds = true;
    _children.clear();
    _children.shrink_to_fit();
    ReorderAllChildren();
    CalcBounds();
}

void Element::RemoveSelf() {
    if(_parent) {
        _parent->RemoveChild(this);
        _parent = nullptr;
    }
}

void Element::DestroyChild(UI::Element*& child) {
    auto iter = std::find_if(std::begin(_children), std::end(_children), [child](UI::Element* c) { return child == c; });
    if(iter != std::end(_children)) {
        delete *iter;
        *iter = nullptr;
    }
}

void Element::DestroyAllChildren() {
    for(auto& iter : _children) {
        iter->_parent = nullptr;
        delete iter;
        iter = nullptr;
    }
    _children.clear();
    _children.shrink_to_fit();
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
    _dirty_bounds = true;
    _position = position;
    CalcBoundsForMeThenMyChildren();
}

void Element::SetPositionRatio(const Vector2& ratio) {
    Element::SetPosition(Vector4{ ratio, _position.GetZW() });
}

void Element::SetPositionOffset(const Vector2& offset) {
    Element::SetPosition(Vector4{ _position.GetXY(), offset });
}

void Element::SetPivot(const Vector2& pivotPosition) {
    _dirty_bounds = true;
    _pivot = pivotPosition;
    CalcBoundsForMeThenMyChildren();
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
        std::ostringstream ss;
        ss << __FUNCTION__ << ": Unhandled pivot mode.";
        ERROR_AND_DIE(ss.str().c_str());
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

void Element::DebugRender(Renderer& renderer, bool showSortOrder /*= false*/) const {
    DebugRenderBoundsAndPivot(renderer);
    if(showSortOrder) {
        DebugRenderOrder(renderer);
    }
}

Matrix4 Element::GetLocalTransform() const noexcept {
    const auto T = Matrix4::CreateTranslationMatrix(CalcLocalPosition());
    const auto R = Matrix4::Create2DRotationMatrix(CalcLocalRotationRadians());
    const auto S = Matrix4::CreateScaleMatrix(CalcLocalScale());
    const auto M = Matrix4::MakeSRT(S, R, T);
    return M;
}

Vector2 Element::CalcLocalScale() const {
    auto my_bounds = CalcLocalBounds();
    auto parent_bounds = GetParentBounds();
    auto parent_width = parent_bounds.maxs.x - parent_bounds.mins.x;
    auto my_width = my_bounds.maxs.x - my_bounds.mins.x;
    auto width_scale = my_width / parent_width;
    auto parent_height = parent_bounds.maxs.y - parent_bounds.mins.y;
    auto my_height = my_bounds.maxs.y - my_bounds.mins.y;
    auto height_scale = my_height / parent_height;
    return _parent ? Vector2(width_scale, height_scale) : _size.unit;
}

Matrix4 Element::GetWorldTransform() const noexcept {
    return Matrix4::MakeRT(GetLocalTransform(), GetParentWorldTransform());
}

Matrix4 Element::GetParentWorldTransform() const noexcept {
    return _parent ? _parent->GetWorldTransform() : Matrix4::I;
}

void Element::DirtyElement() {
    _dirty_bounds = true;
}

void Element::DebugRenderBoundsAndPivot(Renderer& renderer) const {
    DebugRenderBounds(renderer);
    DebugRenderPivot(renderer);
}

void Element::DebugRenderPivot(Renderer& renderer) const {
    const auto world_transform = GetWorldTransform();
    const auto scale = world_transform.GetScale();
    const auto inv_scale_matrix = Matrix4::CalculateInverse(Matrix4::CreateScaleMatrix(Vector3(scale.x * 0.10f, scale.y * 0.10f, 1.0f)));
    const auto extents = GetSize();
    const auto pivot_pos = MathUtils::CalcPointFromNormalizedPoint(_pivot, _bounds);
    const auto pivot_pos_matrix = Matrix4::CreateTranslationMatrix(pivot_pos);
    const auto transform = Matrix4::MakeSRT(inv_scale_matrix, world_transform, pivot_pos_matrix);
    renderer.SetMaterial(renderer.GetMaterial("__2D"));
    renderer.SetModelMatrix(transform);
    renderer.DrawX2D(_pivot_color);
}

void Element::DebugRenderBounds(Renderer& renderer) const {
    auto world_transform = GetWorldTransform();
    renderer.SetModelMatrix(world_transform);
    renderer.SetMaterial(renderer.GetMaterial("__2D"));
    renderer.DrawAABB2(_edge_color, _fill_color);
}

void Element::DebugRenderOrder(Renderer& renderer) const {
    const auto world_transform = GetWorldTransform();
    const auto world_transform_scale = world_transform.GetScale();
    const auto inv_scale_x = 1.0f / world_transform_scale.x;
    const auto inv_scale_y = 1.0f / world_transform_scale.y;
    const auto inv_scale_z = 1.0f / world_transform_scale.z;
    const auto inv_scale = Vector3(inv_scale_x, inv_scale_y, inv_scale_z);
    const auto inv_scale_matrix = Matrix4::CreateScaleMatrix(inv_scale);
    const Vector2 extents = GetSize();
    const Vector2 half_extents = extents * 0.5f;
    const auto inv_half_extents = Vector2(half_extents.x, -half_extents.y);
    const auto font = renderer.GetFont("System32");
    const auto text_height_matrix = Matrix4::CreateTranslationMatrix(Vector2(-16.0f, 32.0f));
    const auto inv_half_extents_matrix = Matrix4::CreateTranslationMatrix(inv_half_extents);
    std::ostringstream ss;
    ss << _order;
    const auto text = ss.str();
    renderer.SetModelMatrix(Matrix4::MakeRT(Matrix4::MakeSRT(text_height_matrix, inv_half_extents_matrix, inv_scale_matrix), world_transform));
    renderer.SetMaterial(font->GetMaterial());
    renderer.DrawTextLine(font, text);
}

AABB2 Element::GetParentBounds() const noexcept {
    return _parent ? _parent->_bounds : AABB2{ 0.0f, 0.0f, _size.unit.x, _size.unit.y };
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

void Element::CalcBounds() noexcept {
    _dirty_bounds = false;
    switch(_mode) {
    case UI::PositionMode::Absolute:
        _bounds = CalcAbsoluteBounds();
        break;
    case UI::PositionMode::Relative:
        _bounds = CalcRelativeBounds();
        break;
    default:
    {
        std::ostringstream ss;
        ss << __FUNCTION__ << ": Unhandled positioning mode.";
        ERROR_AND_DIE(ss.str().c_str());
        break;
    }
    }
}

AABB2 Element::CalcBoundsRelativeToParent() const noexcept {
    Vector2 my_size = GetSize();

    AABB2 parent_bounds = _parent ? _parent->CalcLocalBounds() : CalcLocalBounds();
    Vector2 parent_size = parent_bounds.CalcDimensions();

    Vector2 pivot_position = parent_bounds.mins + (parent_size * _position.GetXY() + _position.GetZW());

    AABB2 my_local_bounds = CalcLocalBounds();
    my_local_bounds.Translate(pivot_position);

    return my_local_bounds;
}

void Element::CalcBoundsForChildren() noexcept {
    for(auto& c : _children) {
        if(c) {
            c->CalcBounds();
        }
    }
}

void Element::CalcBoundsForMeThenMyChildren() noexcept {
    CalcBounds();
    CalcBoundsForChildren();
}

AABB2 Element::CalcRelativeBounds() const noexcept {
    Vector2 size = GetSize();
    Vector2 pivot_position = size * _pivot;

    AABB2 bounds;
    bounds.StretchToIncludePoint(Vector2::ZERO);
    bounds.StretchToIncludePoint(size);
    bounds.Translate(-pivot_position);
    return bounds;
}

AABB2 Element::CalcAbsoluteBounds() const noexcept {
    auto size = GetSize();
    auto parent_bounds = GetParentBounds();
    auto pivot_position = MathUtils::CalcPointFromNormalizedPoint(_pivot, parent_bounds);
    AABB2 bounds;
    bounds.StretchToIncludePoint(Vector2::ZERO);
    bounds.StretchToIncludePoint(size);
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
    auto ratio = _position.GetXY();
    AABB2 alignedBounds = AlignBoundsToContainer(CalcBoundsRelativeToParent(), parent_bounds, ratio);

    auto unit = _position.GetZW();
    Vector2 normalized_ratio = MathUtils::RangeMap(ratio, Vector2(0.0f, 1.0f), Vector2(-1.0f, 1.0f));
    Vector2 scaled_ratio = normalized_ratio * unit;
    Vector2 offset(scaled_ratio);

    alignedBounds.Translate(offset);

    return alignedBounds;
}

AABB2 Element::CalcLocalBounds() const noexcept {
    return { Vector2::ZERO, GetSize() };
}

bool Element::IsDirty() const {
    return _dirty_bounds;
}

bool Element::IsParent() const {
    return !_children.empty();
}

bool Element::IsChild() const {
    return _parent;
}

UI::Canvas* Element::GetParentCanvas() const {
    return _parent_canvas;
}

void Element::SetParentCanvas(UI::Canvas* canvas) {
    _parent_canvas = canvas;
}

void Element::ReorderAllChildren() {
    auto i = GetOrder() + 1;
    for(auto& c : _children) {
        c->_order = i++;
    }
}

void Element::DebugRenderBottomUp(Renderer& renderer, bool showSortOrder /*= false*/) const {
    DebugRenderBoundsAndPivot(renderer);
    if(showSortOrder) {
        DebugRenderOrder(renderer);
    }
    DebugRenderChildren(renderer, showSortOrder);
}

void Element::DebugRenderTopDown(Renderer& renderer, bool showSortOrder /*= false*/) const {
    DebugRenderChildren(renderer);
    DebugRenderBoundsAndPivot(renderer);
    if(showSortOrder) {
        DebugRenderOrder(renderer);
    }
}

void Element::DebugRenderChildren(Renderer& renderer, bool showSortOrder /*= false*/) const {
    for(auto& child : _children) {
        if(child) {
            child->DebugRender(renderer, showSortOrder);
        }
    }
}

AABB2 Element::GetParentLocalBounds() const {
    return _parent ? _parent->CalcLocalBounds() : AABB2(Vector2::ZERO, _size.unit);
}

AABB2 Element::GetParentRelativeBounds() const {
    return _parent ? _parent->CalcBoundsRelativeToParent() : AABB2{ 0.0f, 0.0f, 0.0f, 0.0f };
}

void Element::UpdateChildren(TimeUtils::FPSeconds deltaSeconds) {
    for(auto& child : _children) {
        if(child) {
            child->Update(deltaSeconds);
        }
    }
}

void Element::RenderChildren(Renderer& renderer) const {
    for(auto& child : _children) {
        if(child) {
            child->Render(renderer);
        }
    }
}

AABB2 Element::GetBounds(const AABB2& parent, const Vector4& anchors, const Vector4& offsets) const noexcept {
    Vector2 boundMins = MathUtils::CalcPointFromNormalizedPoint(Vector2(anchors.x, anchors.y), parent) + Vector2(offsets.x, offsets.y);
    Vector2 boundMaxs = MathUtils::CalcPointFromNormalizedPoint(Vector2(anchors.z, anchors.w), parent) + Vector2(offsets.z, offsets.w);
    return AABB2(boundMins, boundMaxs);
}

Vector2 Element::GetSmallestOffset(AABB2 a, AABB2 b) const noexcept {
    auto width = a.CalcDimensions().x;
    auto height = a.CalcDimensions().y;
    auto center = a.CalcCenter();
    b.AddPaddingToSides(-(width * 0.5f), -(height * 0.5f));
    Vector2 closestPoint = MathUtils::CalcClosestPoint(center, b);
    return closestPoint - center;
}

AABB2 Element::MoveToBestFit(const AABB2& obj, const AABB2& container) const noexcept {
    Vector2 offset = GetSmallestOffset(obj, container);
    return obj + offset;
}

float Element::GetAspectRatio() const noexcept {
    auto dims = _bounds.CalcDimensions();
    return dims.x / dims.y;
}

Vector2 Element::GetTopLeft() const noexcept {
    return _bounds.mins;
}

Vector2 Element::GetTopRight() const noexcept {
    return Vector2{ _bounds.maxs.x, _bounds.mins.y };
}

Vector2 Element::GetBottomLeft() const noexcept {
    return Vector2{ _bounds.mins.x, _bounds.maxs.y };
}

Vector2 Element::GetBottomRight() const noexcept {
    return _bounds.maxs;
}

bool Element::HasParent() const {
    return _parent != nullptr;
}

float Element::GetParentOrientationRadians() const {
    return _parent ? _parent->GetOrientationRadians() : 0.0f;
}

float Element::GetParentOrientationDegrees() const {
    return _parent ? _parent->GetOrientationDegrees() : 0.0f;
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

void Element::SetOrder(std::size_t value) {
    _order = value;
}

std::size_t Element::GetOrder() const {
    return _order;
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

Vector2 Element::GetSize() const noexcept {
    return _parent ? (_parent->GetSize() * _size.ratio.GetValue() + _size.unit) : _size.unit;
}

float Element::GetInvAspectRatio() const noexcept {
    return 1.0f / GetAspectRatio();
}

void Element::SetSize(const Metric& size) {
    _dirty_bounds = true;
    _size = size;
    CalcBoundsForMeThenMyChildren();
}

void Element::SortChildren() {
    std::sort(std::begin(_children), std::end(_children), [](UI::Element* a, UI::Element* b) { return a->_order < b->_order; });
}

void Element::SortAllChildren() {
    SortChildren();
    for(auto& c : _children) {
        c->SortAllChildren();
    }
}

} //End UI