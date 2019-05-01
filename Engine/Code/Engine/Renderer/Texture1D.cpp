#include "Engine/Renderer/Texture1D.hpp"

#include "Engine/Core/BuildConfig.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

Texture1D::Texture1D(const RHIDevice* device, ID3D11Texture1D* dxTexture)
    : Texture(device)
    , _dx_tex(dxTexture)
{
    /* DO NOTHING */
}

void Texture1D::SetDebugName([[maybe_unused]] const std::string& name) const noexcept {
#ifdef RENDER_DEBUG
    _dx_tex->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned int>(name.size()), name.data());
#endif
}

Texture1D::~Texture1D() {
    _device = nullptr;
    if(_dx_tex) {
        _dx_tex->Release();
        _dx_tex = nullptr;
    }
}

ID3D11Resource* Texture1D::GetDxResource() const {
    return _dx_tex;
}

Texture1D::Texture1D(Texture1D&& r_other) noexcept
    : Texture(std::move(r_other))
    , _dx_tex(std::move(r_other._dx_tex))
{
    r_other._dx_tex = nullptr;
}

Texture1D& Texture1D::operator=(Texture1D&& rhs) noexcept {
    Texture::operator=(std::move(rhs));
    _dx_tex = std::move(rhs._dx_tex);
    rhs._dx_tex = nullptr;
    return *this;
}
