#include "Engine/Renderer/Texture.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

Texture::Texture(const RHIDevice& device) noexcept
    : _device(device)
{
    /* DO NOTHING */
}

Texture::Texture(Texture&& r_other) noexcept
: _device(r_other._device)
,_dimensions(r_other._dimensions)
,_isLoaded(r_other._isLoaded)
,_isArray(r_other._isArray)
,_dsv(std::move(r_other._dsv))
,_rtv(std::move(r_other._rtv))
,_srv(std::move(r_other._srv))
,_uav(std::move(r_other._uav))
{
    r_other._dsv = nullptr;
    r_other._rtv = nullptr;
    r_other._srv = nullptr;
    r_other._uav = nullptr;
    r_other._dimensions = IntVector3::ZERO;
    r_other._isLoaded = false;
    r_other._isArray = false;
}

Texture& Texture::operator=(Texture&& rhs) noexcept {

    _dimensions = rhs._dimensions;
    _isLoaded = rhs._isLoaded;
    _isArray = rhs._isArray;
    _dsv = std::move(rhs._dsv);
    _rtv = std::move(rhs._rtv);
    _srv = std::move(rhs._srv);
    _uav = std::move(rhs._uav);

    rhs._dsv = nullptr;
    rhs._rtv = nullptr;
    rhs._srv = nullptr;
    rhs._uav = nullptr;
    rhs._dimensions = IntVector3::ZERO;
    rhs._isLoaded = false;
    rhs._isArray = false;

    return *this;
}

Texture::~Texture() = default;

const IntVector3& Texture::GetDimensions() const noexcept {
    return _dimensions;
}

ID3D11Resource* Texture::GetDxResource() const noexcept {
    return nullptr;
}

void Texture::IsLoaded(bool is_loaded) noexcept {
    _isLoaded = is_loaded;
}

bool Texture::IsLoaded() const noexcept {
    return _isLoaded;
}

bool Texture::IsArray() const noexcept {
    return _isArray;
}

ID3D11DepthStencilView* Texture::GetDepthStencilView() const noexcept {
    return _dsv.Get();
}

ID3D11RenderTargetView* Texture::GetRenderTargetView() const noexcept {
    return _rtv.Get();
}

ID3D11ShaderResourceView* Texture::GetShaderResourceView() const noexcept {
    return _srv.Get();
}

ID3D11UnorderedAccessView* Texture::GetUnorderedAccessView() const noexcept {
    return _uav.Get();
}

ID3D11DepthStencilView* Texture::GetDepthStencilView() noexcept {
    return _dsv.Get();
}

ID3D11RenderTargetView* Texture::GetRenderTargetView() noexcept {
    return _rtv.Get();
}

ID3D11ShaderResourceView* Texture::GetShaderResourceView() noexcept {
    return _srv.Get();
}

ID3D11UnorderedAccessView* Texture::GetUnorderedAccessView() noexcept {
    return _uav.Get();
}
