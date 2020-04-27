#include "Engine/UI/Canvas.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"

#include <sstream>
#include <memory>

namespace UI {

Canvas::Canvas(Widget* owner, Renderer& renderer)
: Panel(owner)
, _renderer(renderer)
{

    const auto [dimensions, aspect_ratio] = CalcDimensionsAndAspectRatio();
    CalcDesiredSize();

    auto desc = DepthStencilDesc{};
    desc.stencil_enabled = true;
    desc.stencil_testFront = ComparisonFunction::Equal;
    _renderer.CreateAndRegisterDepthStencilStateFromDepthStencilDescription("UIDepthStencil", desc);
}

 Canvas::Canvas(Widget* owner, Renderer& renderer, const XMLElement& elem)
: Panel(owner)
, _renderer(renderer)
{
     LoadFromXml(elem);
}

void Canvas::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(IsDisabled()) {
        return;
    }
    UpdateChildren(deltaSeconds);
}

void Canvas::Render(Renderer& renderer) const {
    if(IsHidden()) {
        return;
    }
    auto old_camera = renderer.GetCamera();
    SetupMVPFromTargetAndCamera(renderer);
    RenderChildren(renderer);
    renderer.SetCamera(old_camera);
}

void Canvas::SetupMVPFromTargetAndCamera(Renderer& renderer) const {
    SetupMVPFromViewportAndCamera(renderer);
}

void Canvas::SetupMVPFromViewportAndCamera(Renderer& renderer) const {
    renderer.ResetModelViewProjection();
    const auto& vp = renderer.GetCurrentViewport();
    const auto target_dims = Vector2(vp.width, vp.height);
    const Vector2 leftBottom = Vector2(0.0f, 1.0f) * target_dims;
    const Vector2 rightTop = Vector2(1.0f, 0.0f) * target_dims;
    const Vector2 nearFar{0.0f, 1.0f};
    const auto& [dimensions, aspect_ratio] = CalcDimensionsAndAspectRatio();
    _camera.SetupView(leftBottom, rightTop, nearFar, aspect_ratio);
    const Vector2 view_extents{rightTop.x - leftBottom.x, leftBottom.y - rightTop.y};
    const Vector2 view_half_extents{view_extents * 0.5f};
    _camera.SetPosition(view_half_extents);
    renderer.SetCamera(_camera);
    renderer.SetModelMatrix(GetWorldTransform());
}

void Canvas::DebugRender(Renderer& renderer) const {
    const auto& target = _camera.GetRenderTarget();
    renderer.SetRenderTarget(target.color_target, target.depthstencil_target);
    renderer.DisableDepth();
    DebugRenderBottomUp(renderer);
    renderer.EnableDepth();
    renderer.SetRenderTarget();
    renderer.SetMaterial(nullptr);
}

void Canvas::EndFrame() {
    Panel::EndFrame();
    if(IsDirty(InvalidateElementReason::Layout)) {
        ReorderAllChildren();
    }
}

const Camera2D& Canvas::GetUICamera() const {
    return _camera;
}

void Canvas::UpdateChildren(TimeUtils::FPSeconds deltaSeconds) {
    for(auto& slot : _slots) {
        if(auto* child = slot->content) {
            child->Update(deltaSeconds);
        }
    }
}

void Canvas::RenderChildren(Renderer& renderer) const {
    for(auto& slot : _slots) {
        if(auto* child = slot->content) {
            child->Render(renderer);
        }
    }
}

const Renderer& Canvas::GetRenderer() const {
    return _renderer;
}

Renderer& Canvas::GetRenderer() {
    return const_cast<Renderer&>(static_cast<const Canvas&>(*this).GetRenderer());
}

Vector4 Canvas::AnchorTextToAnchorValues(const std::string& text) noexcept {
    const auto values = StringUtils::Split(text, '/');
    GUARANTEE_OR_DIE(values.size() == 2, "UI Anchor Text must be exactly two values separated by a '/'");
    Vector4 anchors{0.5f, 0.5f, 0.5f, 0.5f};
    if(values[0] == "left") {
        anchors.x = 0.0f;
    } else if(values[0] == "center") {
        anchors.x = 0.5f;
    } else if(values[0] == "right") {
        anchors.x = 1.0f;
    } else if(values[0] == "stretchH") {
        if(values[1] == "top") {
            anchors.y = 0.0f;
            return anchors;
        } else if(values[1] == "center") {
            anchors.y = 0.5f;
            return anchors;
        } else if(values[1] == "bottom") {
            anchors.y = 1.0f;
            return anchors;
        } else {
            ERROR_AND_DIE("Ill-formed anchor values.");
        }
    } else if(values[0] == "stretchV") {
        if(values[1] == "left") {
            anchors.y = 0.0f;
            return anchors;
        } else if(values[1] == "center") {
            anchors.y = 0.5f;
            return anchors;
        } else if(values[1] == "right") {
            anchors.y = 1.0f;
            return anchors;
        } else {
            ERROR_AND_DIE("Ill-formed anchor values.");
        }
    } else {
        if(values[0] == "stretch" && values[1] == "both") {
            return Vector4{0.0f, 0.0f, 1.0f, 1.0f};
        }
    }
    if(values[1] == "top") {
        anchors.y = 0.0f;
        return anchors;
    } else if(values[1] == "center") {
        anchors.y = 0.5f;
        return anchors;
    } else if(values[1] == "bottom") {
        anchors.y = 1.0f;
        return anchors;
    } else {
        ERROR_AND_DIE("Ill-formed anchor values.");
    }
}

CanvasSlot* Canvas::AddChild(Element* child) {
    DirtyElement(InvalidateElementReason::Layout);
    auto newSlot = std::make_shared<CanvasSlot>();
    newSlot->content = child;
    newSlot->parent = this;
    auto ptr = newSlot.get();
    _slots.emplace_back(newSlot);
    child->SetSlot(ptr);
    return ptr;
}

CanvasSlot* Canvas::AddChildAt(Element* child, std::size_t index) {
    DirtyElement(InvalidateElementReason::Layout);
    auto newSlot = std::make_shared<CanvasSlot>();
    newSlot->content = child;
    newSlot->parent = this;
    CalcBoundsForMeThenMyChildren();
    auto ptr = newSlot.get();
    _slots[index] = std::move(newSlot);
    if(IsDirty(InvalidateElementReason::Layout)) {
        ReorderAllChildren();
    }
    return ptr;
}

void Canvas::RemoveChild(Element* child) {
    DirtyElement(InvalidateElementReason::Any);
    _slots.erase(
    std::remove_if(std::begin(_slots), std::end(_slots),
                   [child](const decltype(_slots)::value_type& c) {
                       return child == c->content;
                   }),
    std::end(_slots));
    ReorderAllChildren();
    CalcBoundsForMeThenMyChildren();
}

void Canvas::RemoveAllChildren() {
    DirtyElement(InvalidateElementReason::Any);
    _slots.clear();
    _slots.shrink_to_fit();
    CalcBoundsForMeThenMyChildren();
}

Vector4 Canvas::CalcDesiredSize() const noexcept {
    return {};
}

AABB2 Canvas::CalcChildrenDesiredBounds() {
    return {};
}

void Canvas::ArrangeChildren() noexcept {
    /* DO NOTHING */
}

std::pair<Vector2, float> Canvas::CalcDimensionsAndAspectRatio() const {
    const auto& viewport = _renderer.GetCurrentViewport();
    const auto viewport_dims = Vector2{viewport.width, viewport.height};

    const auto target_AR = viewport_dims.x / viewport_dims.y;
    Vector2 dims = Vector2::ZERO;
    if(target_AR <= 1.0f) {
        dims.x = viewport_dims.x;
        dims.y = target_AR * viewport_dims.x;
    } else {
        dims.x = target_AR * viewport_dims.y;
        dims.y = viewport_dims.y;
    }
    return std::make_pair(dims, dims.x / dims.y);
}

AABB2 Canvas::CalcAlignedAbsoluteBounds() const noexcept {

    AABB2 parent_bounds = GetParentLocalBounds();
    auto ratio = GetPosition().GetXY();
    AABB2 alignedBounds = AlignBoundsToContainer(CalcBoundsRelativeToParent(), parent_bounds, ratio);

    auto unit = GetPosition().GetZW();
    Vector2 normalized_ratio = MathUtils::RangeMap(ratio, Vector2(0.0f, 1.0f), Vector2(-1.0f, 1.0f));
    Vector2 scaled_ratio = normalized_ratio * unit;
    Vector2 offset(scaled_ratio);

    alignedBounds.Translate(offset);

    return alignedBounds;
}

void Canvas::ReorderAllChildren() {
    std::sort(std::begin(_slots), std::end(_slots),
    [](const std::shared_ptr<PanelSlot>& a, const std::shared_ptr<PanelSlot>& b) {
        const auto aAsCs = std::dynamic_pointer_cast<CanvasSlot>(a);
        const auto bAsCs = std::dynamic_pointer_cast<CanvasSlot>(b);
        if(aAsCs && bAsCs) {
            return aAsCs->zOrder < bAsCs->zOrder;
        }
        if(aAsCs || bAsCs) {
            if(aAsCs) {
                return aAsCs->zOrder < 0;
            }
            if(bAsCs) {
                return bAsCs->zOrder < 0;
            }
        }
        return false;
    });
}

bool Canvas::LoadFromXml(const XMLElement& elem) noexcept {
    DataUtils::ValidateXmlElement(elem, "canvas", "", "name", "canvas,label,panel,picturebox,button");
    _name = DataUtils::ParseXmlAttribute(elem, "name", _name);
    return true;
}

CanvasSlot::CanvasSlot(const XMLElement& elem)
: PanelSlot()
{
    LoadFromXml(elem);
}

void CanvasSlot::LoadFromXml(const XMLElement& elem) {
    DataUtils::ValidateXmlElement(elem, "slot", "", "", "", "anchors,position,size,alignment,autosize");
    const auto anchorValues = AABB2{Canvas::AnchorTextToAnchorValues(DataUtils::ParseXmlAttribute(elem, "anchors", std::string{"center/center"}))};
    anchors = anchorValues;
    CalcPivot();
    autoSize = DataUtils::ParseXmlAttribute(elem, "autosize", autoSize);
    size = autoSize ? content->CalcDesiredSize().GetZW() : DataUtils::ParseXmlAttribute(elem, "size", Vector2::ZERO);
    position = DataUtils::ParseXmlAttribute(elem, "position", Vector2{0.5f, 0.5f});
    if(auto* xml_parent = elem.Parent()->ToElement()) {
        
    }
}

void CanvasSlot::CalcPivot() {
    const auto desired_size = content->CalcDesiredSize();
    const auto parent_bounds = content->GetParentBounds();
    const auto pivot_position = MathUtils::CalcPointFromNormalizedPoint(content->GetPivot(), parent_bounds);
    this->size = desired_size.GetZW();
    content->SetPivot(pivot_position);
}

} // namespace UI

