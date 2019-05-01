#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"

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

	explicit RenderTargetStack(Renderer* renderer);
    ~RenderTargetStack() = default;

    void Push(const RenderTargetStack::Node& node = RenderTargetStack::Node{});
    void Pop();
    const RenderTargetStack::Node& Top() const;
protected:
private:
    Renderer* _renderer{};
    std::vector<RenderTargetStack::Node> _stack{};
};