#include "Engine/Renderer/TextureArray2D.hpp"


#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"

TextureArray2D::TextureArray2D(const RHIDevice* device, ID3D11Texture2D* dxTexture) noexcept
    : Texture(device)
    , _dx_tex(dxTexture)
{
    SetDeviceAndTexture(device, _dx_tex);
}

void TextureArray2D::SetDebugName([[maybe_unused]] const std::string& name) const noexcept {
#ifdef RENDER_DEBUG
    _dx_tex->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned int>(name.size()), name.data());
#endif
}

TextureArray2D::~TextureArray2D() noexcept {
    _device = nullptr;
    if(_dx_tex) {
        _dx_tex->Release();
        _dx_tex = nullptr;
    }
}

ID3D11Resource* TextureArray2D::GetDxResource() const noexcept {
    return _dx_tex;
}

TextureArray2D::TextureArray2D(TextureArray2D&& r_other) noexcept
    : Texture(std::move(r_other))
    , _dx_tex(std::move(r_other._dx_tex))
{
    r_other._dx_tex = nullptr;
}

TextureArray2D& TextureArray2D::operator=(TextureArray2D&& rhs) noexcept {
    Texture::operator=(std::move(rhs));
    _dx_tex = std::move(rhs._dx_tex);
    rhs._dx_tex = nullptr;
    return *this;
}

void TextureArray2D::SetDeviceAndTexture(const RHIDevice* device, ID3D11Texture2D* texture) noexcept {

    _device = device;
    _dx_tex = texture;

    D3D11_TEXTURE2D_DESC t_desc;
    _dx_tex->GetDesc(&t_desc);
    auto depth = t_desc.ArraySize;
    _dimensions = IntVector3(t_desc.Width, t_desc.Height, depth);
    _isArray = true;

    bool success = true;
    if(t_desc.BindFlags & D3D11_BIND_RENDER_TARGET) {
        D3D11_RENDER_TARGET_VIEW_DESC rtv_desc{};
        rtv_desc.Format = t_desc.Format;
        rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        rtv_desc.Texture2DArray.ArraySize = depth;
        rtv_desc.Texture2DArray.MipSlice = 0;
        rtv_desc.Texture2DArray.FirstArraySlice = 0;
        success &= SUCCEEDED(_device->GetDxDevice()->CreateRenderTargetView(_dx_tex, &rtv_desc, &_rtv)) == true;
    }

    if(t_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
        srv_desc.Format = t_desc.Format;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srv_desc.Texture2DArray.ArraySize = depth;
        srv_desc.Texture2DArray.MipLevels = t_desc.MipLevels;
        srv_desc.Texture2DArray.FirstArraySlice = 0;
        srv_desc.Texture2DArray.MostDetailedMip = 0;
        success &= SUCCEEDED(_device->GetDxDevice()->CreateShaderResourceView(_dx_tex, &srv_desc, &_srv)) == true;
    }

    if(t_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
        D3D11_DEPTH_STENCIL_VIEW_DESC ds_desc{};
        ds_desc.Format = t_desc.Format;
        ds_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        ds_desc.Texture2DArray.ArraySize = depth;
        ds_desc.Texture2DArray.MipSlice = 0;
        ds_desc.Texture2DArray.FirstArraySlice = 0;
        success &= SUCCEEDED(_device->GetDxDevice()->CreateDepthStencilView(_dx_tex, &ds_desc, &_dsv));
    }

    if(t_desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc{};
        uav_desc.Format = t_desc.Format;
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
        uav_desc.Texture2DArray.MipSlice = 0;
        uav_desc.Texture2DArray.FirstArraySlice = 0;
        uav_desc.Texture2DArray.ArraySize = depth;
        success &= SUCCEEDED(_device->GetDxDevice()->CreateUnorderedAccessView(_dx_tex, &uav_desc, &_uav));
    }
    if(!success) {
        if(_dsv) { _dsv->Release(); _dsv = nullptr; }
        if(_rtv) { _rtv->Release(); _rtv = nullptr; }
        if(_srv) { _srv->Release(); _srv = nullptr; }
        if(_uav) { _uav->Release(); _uav = nullptr; }
        ERROR_AND_DIE("Set device and texture failed.");
    }
}