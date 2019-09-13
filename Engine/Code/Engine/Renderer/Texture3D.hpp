#pragma once

#include "Engine/Renderer/Texture.hpp"

class RHIDevice;

struct ID3D11Texture3D;

class Texture3D : public Texture {
public:
    Texture3D(const RHIDevice* device, ID3D11Texture3D* dxTexture) noexcept;
    Texture3D(Texture3D&& r_other) noexcept;
    Texture3D(const Texture3D& other) noexcept = delete;
    Texture3D& operator=(const Texture3D& rhs) noexcept = delete;
    Texture3D& operator=(Texture3D&& rhs) noexcept;

    virtual void SetDebugName([[maybe_unused]] const std::string& name) const noexcept override;

    virtual ~Texture3D();

    virtual ID3D11Resource* GetDxResource() const noexcept override;

protected:
private:
    ID3D11Texture3D* _dx_tex = nullptr;
};