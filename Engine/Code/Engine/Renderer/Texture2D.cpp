#include "Engine/Renderer/Texture2D.hpp"


#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"

Texture2D::Texture2D(const RHIDevice* device, ID3D11Texture2D* dxTexture)
    : Texture(device)
    , _dx_tex(dxTexture)
{
    SetDeviceAndTexture(device, _dx_tex);
}

void Texture2D::SetDebugName([[maybe_unused]] const std::string& name) const noexcept {
#ifdef RENDER_DEBUG
    _dx_tex->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned int>(name.size()), name.data());
#endif
}

Texture2D::~Texture2D() {
    _device = nullptr;
    if(_dx_tex) {
        _dx_tex->Release();
        _dx_tex = nullptr;
    }
}

IntVector2 Texture2D::GetDimensions() const noexcept {
    return IntVector2(this->_dimensions);
}

ID3D11Resource* Texture2D::GetDxResource() const {
    return _dx_tex;
}

ID3D11Texture2D* Texture2D::GetDxTexture() {
    return reinterpret_cast<ID3D11Texture2D*>(this->GetDxResource());
}

Texture2D::Texture2D(Texture2D&& r_other) noexcept
    : Texture(std::move(r_other))
    , _dx_tex(std::move(r_other._dx_tex))
{
    r_other._dx_tex = nullptr;
}

Texture2D& Texture2D::operator=(Texture2D&& rhs) noexcept {
    Texture::operator=(std::move(rhs));
    _dx_tex = std::move(rhs._dx_tex);
    rhs._dx_tex = nullptr;
    return *this;
}

void Texture2D::SetDeviceAndTexture(const RHIDevice* device, ID3D11Texture2D* texture) noexcept {

    _device = device;
    _dx_tex = texture;

    D3D11_TEXTURE2D_DESC t_desc;
    _dx_tex->GetDesc(&t_desc);
    auto depth = t_desc.ArraySize;
    _dimensions = IntVector3(t_desc.Width, t_desc.Height, depth == 1 ? 0 : depth);

    bool success = true;
    if(t_desc.BindFlags & D3D11_BIND_RENDER_TARGET) {
        success &= SUCCEEDED(_device->GetDxDevice()->CreateRenderTargetView(_dx_tex, nullptr, &_rtv)) == true;
    }

    if(bool is_depthstencil = (t_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)) {
        D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
        desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        desc.Flags = 0u;
        bool is_renderable_depthstencil = is_depthstencil && (t_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE);
        desc.Format = is_renderable_depthstencil ? ImageFormatToDxgiFormat(ImageFormat::D32_Float)
                                                 : ImageFormatToDxgiFormat(ImageFormat::D24_UNorm_S8_UInt);
        success &= SUCCEEDED(_device->GetDxDevice()->CreateDepthStencilView(_dx_tex, &desc, &_dsv)) == true;
    }

    if(t_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
        if(_dsv) {
            D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
            desc.Format = DXGI_FORMAT_R32_FLOAT;
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipLevels = 1;
            success &= SUCCEEDED(_device->GetDxDevice()->CreateShaderResourceView(_dx_tex, &desc, &_srv)) == true;
        } else {
            success &= SUCCEEDED(_device->GetDxDevice()->CreateShaderResourceView(_dx_tex, nullptr, &_srv)) == true;
        }
    }

    if(t_desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) {
        D3D11_UNORDERED_ACCESS_VIEW_DESC desc{};
        desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;
        desc.Format = t_desc.Format;

        success &= SUCCEEDED(_device->GetDxDevice()->CreateUnorderedAccessView(_dx_tex, &desc, &_uav)) == true;
    }
    ASSERT_OR_DIE(success, "Set device and texture failed.");
}