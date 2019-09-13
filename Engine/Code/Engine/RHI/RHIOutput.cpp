#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"

#include <sstream>

RHIOutput::RHIOutput(const RHIDevice* parent, Window* wnd, IDXGISwapChain4* swapchain) noexcept
    : _parent_device(parent)
    , _window(wnd)
    , _dxgi_swapchain(swapchain)
{
    CreateBackbuffer();
}

RHIOutput::~RHIOutput() noexcept {
    _parent_device = nullptr;

    if(_dxgi_swapchain) {
        _dxgi_swapchain->Release();
        _dxgi_swapchain = nullptr;
    }

    delete _back_buffer;
    _back_buffer = nullptr;

    delete _window;
    _window = nullptr;

}

const RHIDevice* RHIOutput::GetParentDevice() const noexcept {
    return _parent_device;
}

const Window* RHIOutput::GetWindow() const noexcept {
    return _window;
}

Window* RHIOutput::GetWindow() noexcept {
    return _window;
}

Texture* RHIOutput::GetBackBuffer() noexcept {
    return _back_buffer;
}

IntVector2 RHIOutput::GetDimensions() const noexcept {
    if(_window) {
        return _window->GetDimensions();
    } else {
        return IntVector2::ZERO;
    }
}

float RHIOutput::GetAspectRatio() const noexcept {
    if(_window) {
        const auto& dims = GetDimensions();
        if(dims.y < dims.x) {
            return dims.x / static_cast<float>(dims.y);
        } else {
            return dims.y / static_cast<float>(dims.x);
        }
    }
    return 0.0f;
}

void RHIOutput::SetDisplayMode(const RHIOutputMode& newMode) noexcept {
    _window->SetDisplayMode(newMode);
}

void RHIOutput::SetDimensions(const IntVector2& clientSize) noexcept {
    _window->SetDimensions(clientSize);
}

void RHIOutput::Present(bool vsync) noexcept {
    DXGI_PRESENT_PARAMETERS present_params{};
    present_params.DirtyRectsCount = 0;
    present_params.pDirtyRects = nullptr;
    present_params.pScrollOffset = nullptr;
    present_params.pScrollRect = nullptr;
    bool should_tear = _parent_device->IsAllowTearingSupported();
    bool is_vsync_off = !vsync;
    bool use_no_sync_interval = should_tear && is_vsync_off;
    unsigned int sync_interval = use_no_sync_interval ? 0u : 1u;
    unsigned int present_flags = use_no_sync_interval ? DXGI_PRESENT_ALLOW_TEARING : 0;
    auto hr_present = _dxgi_swapchain->Present1(sync_interval, present_flags, &present_params);
    #ifdef RENDER_DEBUG
    std::ostringstream ss;
    ss << "Present call failed: " << hr_present;
    GUARANTEE_OR_DIE(SUCCEEDED(hr_present), ss.str().c_str());
    #else
    GUARANTEE_OR_DIE(SUCCEEDED(hr_present), "Present call failed.");
    #endif
}

void RHIOutput::CreateBackbuffer() noexcept {
    if(_back_buffer != nullptr) {
        delete _back_buffer;
    }
    ID3D11Texture2D* back_buffer = nullptr;
    _dxgi_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
    _back_buffer = new Texture2D(_parent_device, back_buffer);
    _back_buffer->SetDebugName("__back_buffer");
}

void RHIOutput::ResetBackbuffer() noexcept {
    delete _back_buffer;
    _back_buffer = nullptr;
    CreateBackbuffer();
}
