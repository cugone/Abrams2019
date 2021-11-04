#include "Engine/Renderer/FrameBuffer.hpp"

#include "Engine/Platform/DirectX/DirectX11FrameBuffer.hpp"

std::shared_ptr<FrameBuffer> FrameBuffer::Create(FrameBufferDesc& desc) noexcept {

std::shared_ptr<FrameBuffer> FrameBuffer::Create(const FrameBufferDesc& desc) noexcept {
    //TODO: API selection
    return std::make_shared<DirectX11FrameBuffer>(desc);
}
