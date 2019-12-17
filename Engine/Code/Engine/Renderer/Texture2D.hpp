#pragma once

#include "Engine/Renderer/Texture.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

class RHIDevice;
class IntVector2;

class Texture2D : public Texture {
public:
    Texture2D(const RHIDevice& device, Microsoft::WRL::ComPtr<ID3D11Texture2D> dxTexture) noexcept;
    Texture2D(Texture2D&& r_other) noexcept;
    Texture2D(const Texture2D& other) noexcept = delete;
    Texture2D& operator=(const Texture2D& rhs) noexcept = delete;
    Texture2D& operator=(Texture2D&& rhs) noexcept;

    virtual void SetDebugName([[maybe_unused]] const std::string& name) const noexcept override;

    virtual ~Texture2D() noexcept = default;

    IntVector2 GetDimensions() const noexcept;

    virtual ID3D11Resource* GetDxResource() const noexcept override;
    ID3D11Texture2D* GetDxTexture() noexcept;
protected:
private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _dx_tex{};
    void SetTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture) noexcept;
};