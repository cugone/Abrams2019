#pragma once

#include "Engine/Math/IntVector3.hpp"

#include <string>

class RHIDevice;

struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;
struct ID3D11Resource;

class Texture {
public:
    Texture(const RHIDevice* device) noexcept;
    Texture(Texture&& r_other) noexcept;
    Texture(const Texture& other) noexcept = delete;
    Texture& operator=(const Texture& rhs) noexcept = delete;
    Texture& operator=(Texture&& rhs) noexcept;
    virtual ~Texture() noexcept = 0;

    const IntVector3& GetDimensions() const noexcept;

    void IsLoaded(bool is_loaded) noexcept;
    bool IsLoaded() const noexcept;

    bool IsArray() const noexcept;

    ID3D11DepthStencilView* GetDepthStencilView() noexcept;
    ID3D11RenderTargetView* GetRenderTargetView() noexcept;
    ID3D11ShaderResourceView* GetShaderResourceView() noexcept;
    ID3D11UnorderedAccessView* GetUnorderedAccessView() noexcept;
    ID3D11DepthStencilView* GetDepthStencilView() const noexcept;
    ID3D11RenderTargetView* GetRenderTargetView() const noexcept;
    ID3D11ShaderResourceView* GetShaderResourceView() const noexcept;
    ID3D11UnorderedAccessView* GetUnorderedAccessView() const noexcept;

    virtual void SetDebugName([[maybe_unused]] const std::string& name) const noexcept = 0;
    virtual ID3D11Resource* GetDxResource() const noexcept = 0;

protected:
    const RHIDevice* _device = nullptr;
    IntVector3 _dimensions = IntVector3::ZERO;
    ID3D11DepthStencilView* _dsv = nullptr;
    ID3D11RenderTargetView* _rtv = nullptr;
    ID3D11ShaderResourceView* _srv = nullptr;
    ID3D11UnorderedAccessView* _uav = nullptr;
    bool _isLoaded = false;
    bool _isArray = false;
private:
};
