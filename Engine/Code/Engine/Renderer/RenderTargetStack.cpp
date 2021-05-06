#include "Engine/Renderer/RenderTargetStack.hpp"

#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/Renderer.hpp"

bool operator==(const RenderTargetStack::Node& lhs, const RenderTargetStack::Node& rhs) {
    return lhs.color_target == rhs.color_target && lhs.depthstencil_target == rhs.depthstencil_target && lhs.view_desc == rhs.view_desc;
}

bool operator!=(const RenderTargetStack::Node& lhs, const RenderTargetStack::Node& rhs) {
    return !(lhs == rhs);
}

RenderTargetStack::RenderTargetStack(Renderer& renderer) noexcept
: _renderer(renderer) {
    /* DO NOTHING */
}

[[nodiscard]] bool RenderTargetStack::empty() const {
    return _stack.empty();
}

[[nodiscard]] std::size_t RenderTargetStack::size() const {
    return _stack.size();
}

void RenderTargetStack::push(const RenderTargetStack::Node& node) noexcept {
    _stack.push(node);
    const auto& top = _stack.top();
    _renderer.SetRenderTarget(top.color_target, top.depthstencil_target);
    const auto x = static_cast<unsigned int>(top.view_desc.x);
    const auto y = static_cast<unsigned int>(top.view_desc.y);
    const auto w = static_cast<unsigned int>(top.view_desc.width);
    const auto h = static_cast<unsigned int>(top.view_desc.height);
    _renderer.SetViewport(x, y, w, h);
}

void RenderTargetStack::push(RenderTargetStack::Node&& node) noexcept {
    _stack.push(node);
    const auto& top = _stack.top();
    _renderer.SetRenderTarget(top.color_target, top.depthstencil_target);
    const auto x = static_cast<unsigned int>(top.view_desc.x);
    const auto y = static_cast<unsigned int>(top.view_desc.y);
    const auto w = static_cast<unsigned int>(top.view_desc.width);
    const auto h = static_cast<unsigned int>(top.view_desc.height);
    _renderer.SetViewport(x, y, w, h);
}

void RenderTargetStack::pop() noexcept {
    _stack.pop();
    const auto& top = _stack.top();
    _renderer.SetRenderTarget(top.color_target, top.depthstencil_target);
    _renderer.ClearColor(Rgba::Black);
    _renderer.ClearDepthStencilBuffer();
    const auto x = static_cast<unsigned int>(top.view_desc.x);
    const auto y = static_cast<unsigned int>(top.view_desc.y);
    const auto w = static_cast<unsigned int>(top.view_desc.width);
    const auto h = static_cast<unsigned int>(top.view_desc.height);
    _renderer.SetViewport(x, y, w, h);
}

[[nodiscard]] RenderTargetStack::Node& RenderTargetStack::top() noexcept {
    return _stack.top();
}

[[nodiscard]] const RenderTargetStack::Node& RenderTargetStack::top() const noexcept {
    return _stack.top();
}
