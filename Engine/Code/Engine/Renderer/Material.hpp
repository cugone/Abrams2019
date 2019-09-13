#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

class Renderer;
class Shader;
class Texture;

class Material {
public:
    enum class TextureID : std::size_t {
        Diffuse
        ,Normal
        ,Displacement
        ,Specular
        ,Occlusion
        ,Emissive
        ,Custom1
        ,Custom2
        ,Custom3
        ,Custom4
        ,Custom5
        ,Custom6
        ,Custom7
        ,Custom8
        ,Custom9
        ,Custom10
        ,Custom11
        ,Custom12
        ,Custom13
        ,Custom14
        ,Custom15
        ,Custom16
        ,Custom17
        ,Custom18
        ,Custom19
        ,Custom20
        ,Custom21
        ,Custom22
        ,Custom23
        ,Custom24
        ,Custom25
        ,Custom26
        ,Custom27
        ,Custom28
        ,Custom29
        ,Custom30
        ,Custom31
        ,Custom32
        ,Custom33
        ,Custom34
        ,Custom35
        ,Custom36
        ,Custom37
        ,Custom38
        ,Custom39
        ,Custom40
        ,Custom41
        ,Custom42
        ,Custom43
        ,Custom44
        ,Custom45
        ,Custom46
        ,Custom47
        ,Custom48
        ,Custom49
        ,Custom50
        ,Custom51
        ,Custom52
        ,Custom53
        ,Custom54
        ,Custom55
        ,Custom56
        ,Custom57
        ,Custom58
    };

    explicit Material(Renderer* renderer) noexcept;
    Material(Renderer* renderer, const XMLElement& element) noexcept;
    ~Material() = default;

    std::string GetName() const noexcept;
    Shader * GetShader() const noexcept;
    std::size_t GetTextureCount() const noexcept;
    Texture* GetTexture(std::size_t i) const noexcept;
    Texture* GetTexture(const TextureID& id) const noexcept;
    float GetSpecularIntensity() const noexcept;
    float GetGlossyFactor() const noexcept;
    float GetEmissiveFactor() const noexcept;
    Vector3 GetSpecGlossEmitFactors() const noexcept;

protected:
private:
    bool LoadFromXml(const XMLElement& element) noexcept;
    void AddTextureSlots(std::size_t count) noexcept;
    void AddTextureSlot() noexcept;

    constexpr static std::size_t CustomTextureIndexSlotOffset = 6u;
    constexpr static std::size_t MaxCustomTextureSlotCount = (D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT / 2) - CustomTextureIndexSlotOffset;
    float _specularIntensity = 1.0f;
    float _specularPower = 8.0f;
    float _emissiveFactor = 0.0f;
    std::string _name = "MATERIAL";
    Renderer* _renderer = nullptr;
    std::vector<Texture*> _textures = {};
    Shader* _shader = nullptr;
};