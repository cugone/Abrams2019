#pragma once

#include "Engine/Core/Rgba.hpp"

#include <filesystem>
#include <memory>
#include <vector>

class Renderer;
class Material;

namespace FileUtils {

class MtlReader {
public:
    MtlReader() noexcept = delete;
    explicit MtlReader(Renderer& renderer) noexcept;
    MtlReader(const MtlReader& other) = default;
    MtlReader(MtlReader&& other) = default;
    MtlReader& operator=(const MtlReader& rhs) = default;
    MtlReader& operator=(MtlReader&& rhs) = default;
    explicit MtlReader(Renderer& renderer, std::filesystem::path filepath) noexcept;
    ~MtlReader() = default;

    [[nodiscard]] bool Load(std::filesystem::path filepath) noexcept;
    [[nodiscard]] bool Parse(std::filesystem::path filepath) noexcept;
    [[nodiscard]] bool Parse() noexcept;

    [[nodiscard]] std::vector<Material*> GetMaterials() noexcept;

protected:
private:
    void PrintErrorToDebugger(std::filesystem::path filepath, const std::string& elementType, unsigned long long line_index) const noexcept;

    enum class IlluminationModel : uint8_t {
        ColorNoAmbient,
        ColorAmbient,
        Highlight,
        RayTrace,
        GlassRayTrace,
        FresnelRayTrace,
        RefractionRayTrace,
        RefractionFresnelRayTrace,
        ReflectionNoRayTrace,
        GlassNoRayTrace,
        ShadowsOnInvisible
    };
    struct IlluminationOptions {
        uint8_t color : 1;
        uint8_t ambient : 1;
        uint8_t highlight : 1;
        uint8_t reflection : 1;
        uint8_t raytrace : 1;
        uint8_t transparency : 1;
        uint8_t glass : 1;
        uint8_t refraction : 1;
        uint8_t fresnel : 1;
        uint8_t castOnInvisible : 1;
    };
    Rgba m_ambientColor{};
    Rgba m_diffuseColor{};
    Rgba m_specularColor{};
    Rgba m_transmissionFilterColor{};
    float m_specularExponent{};
    float m_transparencyWeight{};
    float m_indexOfRefraction{};

    Renderer& _renderer;
    std::vector<Material*> m_materials{};
    IlluminationOptions m_lightOptions{};
};

} // namespace FileUtils
