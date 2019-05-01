#include "Engine/UI/Canvas.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include <sstream>

namespace UI {

Canvas::Canvas(Renderer& renderer, float reference_resolution, Texture* target_texture /*= nullptr*/, Texture* target_depthStencil /*= nullptr*/)
    : Element()
    , _renderer(&renderer)
    , _target_texture(target_texture)
    , _target_depthstencil(target_depthStencil)
    , _reference_resolution(reference_resolution)
{
    if(!_target_texture) {
        _target_texture = _renderer->GetOutput()->GetBackBuffer();
    }
    if(!_target_depthstencil) {
        _target_depthstencil = _renderer->GetDefaultDepthStencil();
    }
    {
        std::ostringstream ss;
        ss << __FUNCTION__ << ": reference resolution must not be zero.";
        GUARANTEE_OR_DIE(!MathUtils::IsEquivalent(_reference_resolution, 0.0f), ss.str().c_str());
    }

    Vector2 dimensions{};
    CalcDimensionsAndAspectRatio(dimensions, _aspect_ratio);
    SetSize(Metric{ Ratio{}, dimensions });

    auto desc = DepthStencilDesc{};
    desc.stencil_enabled = true;
    desc.stencil_testFront = ComparisonFunction::Equal;
    _renderer->CreateAndRegisterDepthStencilStateFromDepthStencilDescription("UIDepthStencil", desc);
}

void Canvas::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(IsDisabled()) {
        return;
    }
    UpdateChildren(deltaSeconds);
}

void Canvas::Render(Renderer* renderer) const {
    if(IsHidden()) {
        return;
    }
    auto old_camera = renderer->GetCamera();
    SetupMVPFromTargetAndCamera(renderer);
    renderer->SetRenderTarget(_target_texture, _target_depthstencil);
    RenderChildren(renderer);
    renderer->SetCamera(old_camera);
}

void Canvas::SetupMVPFromTargetAndCamera(Renderer* renderer) const {
    auto texture_dims = _target_texture->GetDimensions();
    auto target_dims = Vector2((float)texture_dims.x, (float)texture_dims.y);
    Vector2 leftBottom = Vector2(0.0f, 1.0f) * target_dims;
    Vector2 rightTop = Vector2(1.0f, 0.0f) * target_dims;
    Vector2 nearFar{ 0.0f, 1.0f };
    _camera.SetupView(leftBottom, rightTop, nearFar, _aspect_ratio);
    renderer->SetCamera(_camera);
    renderer->SetModelMatrix(GetWorldTransform());
}

void Canvas::DebugRender(Renderer* renderer, bool showSortOrder /*= false*/) const {
    renderer->SetRenderTarget(_target_texture, _target_depthstencil);
    renderer->DisableDepth();
    DebugRenderBottomUp(renderer, showSortOrder);
    renderer->EnableDepth();
    renderer->SetRenderTarget();
    renderer->SetMaterial(nullptr);
}

const Camera2D& Canvas::GetUICamera() const {
    return _camera;
}

void Canvas::CalcDimensionsAndAspectRatio(Vector2& dimensions, float& aspectRatio) {
    if(!_target_texture) {
        _target_texture = _renderer->GetOutput()->GetBackBuffer();
    }
    auto texture_dims = _target_texture->GetDimensions();
    Vector2 target_dims = Vector2((float)texture_dims.x, (float)texture_dims.y);

    auto target_AR = target_dims.x / target_dims.y;
    Vector2 dims = Vector2::ZERO;
    if(target_AR <= 1.0f) {
        dims.x = _reference_resolution;
        dims.y = target_AR * _reference_resolution;
    } else {
        dims.x = target_AR * _reference_resolution;
        dims.y = _reference_resolution;
    }
    aspectRatio = dims.x / dims.y;
    dimensions = dims;
}


void Canvas::SetTargetTexture(Renderer& renderer, Texture* target, Texture* depthstencil) {
    _renderer = &renderer;
    if(!target) {
        target = _renderer->GetOutput()->GetBackBuffer();
    }
    if(!depthstencil) {
        depthstencil = _renderer->GetDefaultDepthStencil();
    }
    _target_texture = target;
    _target_depthstencil = depthstencil;

    Vector2 dimensions{};
    CalcDimensionsAndAspectRatio(dimensions, _aspect_ratio);
    Metric m;
    m.ratio = Ratio(Vector2::ZERO);
    m.unit = dimensions;
    SetSize(m);
}

} //End UI