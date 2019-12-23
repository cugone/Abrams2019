#pragma once

#include "Engine/Renderer/Texture.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

class RHIDevice;

class TextureArray2D : public Texture {
public:
    TextureArray2D(const RHIDevice& device, Microsoft::WRL::ComPtr<ID3D11Texture2D> dxTexture) noexcept;
    TextureArray2D(TextureArray2D&& r_other) noexcept;
    TextureArray2D(const TextureArray2D& other) noexcept = delete;
    TextureArray2D& operator=(const TextureArray2D& rhs) noexcept = delete;
    TextureArray2D& operator=(TextureArray2D&& rhs) noexcept;

    virtual void SetDebugName([[maybe_unused]] const std::string& name) const noexcept override;

    virtual ~TextureArray2D() noexcept = default;

    virtual ID3D11Resource* GetDxResource() const noexcept override;

protected:
private:
    void SetTexture() noexcept;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> _dx_tex{};
};