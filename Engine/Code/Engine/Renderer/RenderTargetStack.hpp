#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"

#include <stack>
#include <vector>

class DepthStencilState;
class Renderer;
class Texture;

class RenderTargetStack {
public:

    struct Node {
        Texture* color_target{};
        Texture* depthstencil_target{};
        ViewportDesc view_desc{};
    };

	explicit RenderTargetStack(Renderer* renderer) noexcept;
    ~RenderTargetStack() = default;

    const RenderTargetStack::Node& top() const noexcept;
    RenderTargetStack::Node& top() noexcept;
    void push(const RenderTargetStack::Node& node) noexcept;
    void push(RenderTargetStack::Node&& node) noexcept;
    void pop() noexcept;
    [[nodiscard]] bool empty() const;
    std::size_t size() const;

protected:
private:
    Renderer* _renderer{};
    std::stack<RenderTargetStack::Node, std::vector<RenderTargetStack::Node>> _stack{};
};

bool operator==(const RenderTargetStack::Node& lhs, const RenderTargetStack::Node& rhs);
bool operator!=(const RenderTargetStack::Node& lhs, const RenderTargetStack::Node& rhs);
