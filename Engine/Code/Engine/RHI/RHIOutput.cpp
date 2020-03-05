#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/DirectX/DX11.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/Window.hpp"

#include <sstream>

RHIOutput::RHIOutput(const RHIDevice& parent, std::unique_ptr<Window> wnd) noexcept
: _parent_device(parent)
, _window(std::move(wnd)) {
    CreateBuffers();
}

const RHIDevice& RHIOutput::GetParentDevice() const noexcept {
    return _parent_device;
}

const Window* RHIOutput::GetWindow() const noexcept {
    return _window.get();
}

Window* RHIOutput::GetWindow() noexcept {
    return _window.get();
}

Texture* RHIOutput::GetBackBuffer() const noexcept {
    return _back_buffer.get();
}

Texture* RHIOutput::GetDepthStencil() const noexcept {
    return _depthstencil.get();
}

Texture* RHIOutput::GetFullscreenTexture() const noexcept {
    return _fullscreen.get();
}

IntVector2 RHIOutput::GetDimensions() const noexcept {
    if(_window) {
        return _window->GetClientDimensions();
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
    bool should_tear = _parent_device.IsAllowTearingSupported();
    bool is_vsync_off = !vsync;
    bool use_no_sync_interval = should_tear && is_vsync_off;
    unsigned int sync_interval = use_no_sync_interval ? 0u : 1u;
    unsigned int present_flags = use_no_sync_interval ? DXGI_PRESENT_ALLOW_TEARING : 0;
    auto hr_present = _parent_device.GetDxSwapChain()->Present1(sync_interval, present_flags, &present_params);
#ifdef RENDER_DEBUG
    const auto err_str = std::string{"Present call failed: "} + StringUtils::FormatWindowsMessage(hr_present);
    GUARANTEE_OR_DIE(SUCCEEDED(hr_present), err_str.c_str());
#else
    GUARANTEE_OR_DIE(SUCCEEDED(hr_present), "Present call failed.");
#endif
}

void RHIOutput::CreateBuffers() noexcept {
    _back_buffer = CreateBackbuffer();
    _back_buffer->SetDebugName("__back_buffer");

    _depthstencil = CreateDepthStencil();
    _depthstencil->SetDebugName("__default_depthstencil");

    _fullscreen = CreateFullscreenTexture();
    _fullscreen->SetDebugName("__fullscreen");
}

std::unique_ptr<Texture> RHIOutput::CreateBackbuffer() noexcept {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer{};
    _parent_device.GetDxSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), static_cast<void**>(&back_buffer));
    return std::make_unique<Texture2D>(_parent_device, back_buffer);
}

std::unique_ptr<Texture> RHIOutput::CreateDepthStencil() noexcept {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthstencil{};

    D3D11_TEXTURE2D_DESC descDepth{};
    const auto dims = GetBackBuffer()->GetDimensions();
    const auto width = dims.x;
    const auto height = dims.y;
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = ImageFormatToDxgiFormat(ImageFormat::D24_UNorm_S8_UInt);
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = BufferUsageToD3DUsage(BufferUsage::Default);
    descDepth.BindFlags = BufferBindUsageToD3DBindFlags(BufferBindUsage::Depth_Stencil);
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    auto hr_texture = _parent_device.GetDxDevice()->CreateTexture2D(&descDepth, nullptr, &depthstencil);
    if(FAILED(hr_texture)) {
        std::string error_str{"Fatal Error: Failed to create depthstencil for window. Reason:\n"};
        error_str += StringUtils::FormatWindowsMessage(hr_texture);
        ERROR_AND_DIE(error_str.c_str());
    }
    return std::make_unique<Texture2D>(_parent_device, depthstencil);
}

std::unique_ptr<Texture> RHIOutput::CreateFullscreenTexture() noexcept {
    D3D11_TEXTURE2D_DESC tex_desc{};

    const auto dims = GetBackBuffer()->GetDimensions();
    const auto width = dims.x;
    const auto height = dims.y;
    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    const auto bufferUsage = BufferUsage::Gpu;
    const auto imageFormat = ImageFormat::R8G8B8A8_UNorm;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    const auto bindUsage = BufferBindUsage::Render_Target | BufferBindUsage::Shader_Resource;
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::Unordered_Access) == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
    }
    if((bufferUsage & BufferUsage::Staging) == BufferUsage::Staging) {
        tex_desc.BindFlags = 0;
    }
    tex_desc.MiscFlags = 0;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data{};
    auto data = std::vector<Rgba>(static_cast<std::size_t>(dims.x) * static_cast<std::size_t>(dims.y), Rgba::Magenta);
    subresource_data.pSysMem = data.data();
    subresource_data.SysMemPitch = width * sizeof(Rgba);
    subresource_data.SysMemSlicePitch = width * height * sizeof(Rgba);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> dx_tex{};

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::Static;
    bool mustUseInitialData = isImmutable || isMultiSampled;

    auto hr = _parent_device.GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    if(FAILED(hr)) {
        std::string error_str{"Fatal Error: Failed to create fullscreen texture. Reason:\n"};
        error_str += StringUtils::FormatWindowsMessage(hr);
        ERROR_AND_DIE(error_str.c_str());
    }
    return std::make_unique<Texture2D>(_parent_device, dx_tex);
}

void RHIOutput::SetTitle(const std::string& newTitle) const noexcept {
    _window->SetTitle(newTitle);
}

void RHIOutput::ResetBackbuffer() noexcept {
    _back_buffer.reset();
    _depthstencil.reset();
    _fullscreen.reset();
    _parent_device.ResetSwapChainForHWnd();
    CreateBuffers();
}
