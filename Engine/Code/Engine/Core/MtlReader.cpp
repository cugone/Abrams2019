#include "Engine/Core/MtlReader.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Profiling/ProfileLogScope.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Material.hpp"

#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace FileUtils {

    [[nodiscard]] std::optional<Rgba> GetColor(const std::string& cur_line) noexcept;
    void PrintErrorToDebugger(std::filesystem::path& filepath, std::string_view elementType, unsigned long long line_index) noexcept;
    [[nodiscard]] bool GetColorEntry(Rgba& color, const std::string& cur_line, std::string_view element_id, std::filesystem::path filepath, bool already_seen, unsigned long long last_seen_on_line, unsigned long long line_index);
    [[nodiscard]] std::filesystem::path GetTexturePath(const std::filesystem::path& filepath, const std::string& cur_line) noexcept;

    template<typename T>
    [[nodiscard]] T GetSingleValue(const std::string& cur_line, const std::size_t element_length) noexcept {
        auto value = T{};
        std::stringstream d_ss{};
        d_ss << cur_line.substr(element_length);
        d_ss.clear();
        d_ss.seekg(0);
        d_ss.seekp(0);
        d_ss >> value;
        return value;
    }

    MtlReader::MtlReader(Renderer& renderer) noexcept
        : _renderer(renderer)
    {
        /* DO NOTHING */
    }

    MtlReader::MtlReader(Renderer& renderer, std::filesystem::path filepath) noexcept
        : _renderer(renderer)
    {
        namespace FS = std::filesystem;
        {
            const auto error_msg = std::string{"MtlReader: "} + filepath.string() + " failed to load.\nReason: It does not exist.\n";
            GUARANTEE_OR_DIE(FS::exists(filepath), error_msg.c_str());
        } // namespace std::filesystem;
        filepath = FS::canonical(filepath);
        filepath.make_preferred();
        {
            const auto error_msg = std::string{"MtlReader: "} + filepath.string() + " failed to load.";
            GUARANTEE_OR_DIE(Load(filepath), error_msg.c_str());
        }
    }

    bool MtlReader::Load(std::filesystem::path filepath) noexcept {
        PROFILE_LOG_SCOPE_FUNCTION();

        namespace FS = std::filesystem;
        bool not_exist = !FS::exists(filepath);
        std::string valid_extension = ".mtl";
        bool not_mtl = StringUtils::ToLowerCase(filepath.extension().string()) != valid_extension;
        bool invalid = not_exist || not_mtl;
        if(invalid) {
            DebuggerPrintf("%s is not a .mtl file.\n", filepath.string().c_str());
            return false;
        }
        filepath = FS::canonical(filepath);
        filepath.make_preferred();
        return Parse(filepath);
    }

    bool MtlReader::Parse(std::filesystem::path filepath) noexcept {
        if(auto buffer = FileUtils::ReadBinaryBufferFromFile(filepath); buffer.has_value()) {
            if(std::stringstream ss{}; ss.write(reinterpret_cast<const char*>(buffer->data()), buffer->size())) {
                buffer->clear();
                buffer->shrink_to_fit();
                ss.clear();
                ss.seekg(ss.beg);
                ss.seekp(ss.beg);
                std::string cur_line{};
                unsigned long long line_index = 0ull;

                while(std::getline(ss, cur_line, '\n')) {
                    ++line_index;
                    cur_line = cur_line.substr(0, cur_line.find_first_of('#'));
                    if(cur_line.empty()) {
                        continue;
                    }
                    cur_line = StringUtils::TrimWhitespace(cur_line);
                    if(StringUtils::StartsWith(cur_line, "newmtl ")) {
                        auto material = std::make_unique<Material>(_renderer);
                        auto* ptr = material.get();
                        ptr->_name = cur_line.substr(7);
                        _renderer.RegisterMaterial(std::move(material));
                        m_materials.emplace_back(ptr);
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "d ")) {
                        m_transparencyWeight = GetSingleValue<float>(cur_line, std::size_t{2u});
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "Ns ")) {
                        m_materials.back()->_specularPower = GetSingleValue<float>(cur_line, std::size_t{3u});
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "Ka ")) {
                        static bool already_seen{false};
                        static unsigned long long last_seen{line_index};
                        if(!GetColorEntry(m_ambientColor, cur_line, "Ka", filepath, already_seen, last_seen, line_index)) {
                            return false;
                        }
                        last_seen = line_index;
                        already_seen = true;
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "Kd ")) {
                        static bool already_seen{false};
                        static unsigned long long last_seen{line_index};
                        if(!GetColorEntry(m_diffuseColor, cur_line, "Kd", filepath, already_seen, last_seen, line_index)) {
                            return false;
                        }
                        last_seen = line_index;
                        already_seen = true;
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "Ks ")) {
                        static bool already_seen{false};
                        static unsigned long long last_seen{line_index};
                        if(!GetColorEntry(m_specularColor, cur_line, "Ks", filepath, already_seen, last_seen, line_index)) {
                            return false;
                        }
                        last_seen = line_index;
                        already_seen = true;
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "Ke ")) {
                        static bool already_seen{false};
                        static unsigned long long last_seen{line_index};
                        if(!GetColorEntry(m_emissiveColor, cur_line, "Ke", filepath, already_seen, last_seen, line_index)) {
                            return false;
                        }
                        last_seen = line_index;
                        already_seen = true;
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "Tf ")) {
                        static bool already_seen{false};
                        static unsigned long long last_seen{line_index};
                        if(!GetColorEntry(m_transmissionFilterColor, cur_line, "Tf", filepath, already_seen, last_seen, line_index)) {
                            return false;
                        }
                        m_transmissionFilterColor.InvertRGB();
                        last_seen = line_index;
                        already_seen = true;
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "Ni ")) {
                        m_indexOfRefraction = std::clamp(GetSingleValue<float>(cur_line, std::size_t{3u}), 0.001f, 10.0f);
                        DebuggerPrintf("MtlReader: Optical Density (index of refraction) not supported.\n");
                        PrintErrorToDebugger(filepath, "Ni", line_index);
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "sharpness ")) {
                        m_sharpness = std::clamp(GetSingleValue<int>(cur_line, std::size_t{10u}), 0, 1000);
                        DebuggerPrintf("MtlReader: sharpness not supported.\n");
                        PrintErrorToDebugger(filepath, "sharpness", line_index);
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "map_Ka ")) {
                        const auto path = GetTexturePath(filepath, cur_line);
                        if(auto* ambient = _renderer.CreateOrGetTexture(path, IntVector3::XY_AXIS)) {
                            m_materials.back()->SetTextureSlot(Material::TextureID::Diffuse, ambient);
                        } else {
                            DebuggerPrintf("MtlReader: Ambient texture not found.\n");
                            PrintErrorToDebugger(filepath, "map_Ka", line_index);
                            return false;
                        }
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "map_Kd ")) {
                        const auto path = GetTexturePath(filepath, cur_line);
                        if(auto* diffuse = _renderer.CreateOrGetTexture(path, IntVector3::XY_AXIS)) {
                            m_materials.back()->SetTextureSlot(Material::TextureID::Diffuse, diffuse);
                        } else {
                            DebuggerPrintf("MtlReader: Diffuse texture not found.\n");
                            PrintErrorToDebugger(filepath, "map_Kd", line_index);
                            return false;
                        }
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "map_Ks ")) {
                        const auto path = GetTexturePath(filepath, cur_line);
                        if(auto* specular = _renderer.CreateOrGetTexture(path, IntVector3::XY_AXIS)) {
                            m_materials.back()->SetTextureSlot(Material::TextureID::Specular, specular);
                        } else {
                            DebuggerPrintf("MtlReader: Specular texture not found.\n");
                            PrintErrorToDebugger(filepath, "map_Ks", line_index);
                            return false;
                        }
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "map_Ke ")) {
                        const auto path = GetTexturePath(filepath, cur_line);
                        if(auto* emissive = _renderer.CreateOrGetTexture(path, IntVector3::XY_AXIS)) {
                            m_materials.back()->SetTextureSlot(Material::TextureID::Emissive, emissive);
                        } else {
                            DebuggerPrintf("MtlReader: Emissive texture not found.\n");
                            PrintErrorToDebugger(filepath, "map_Ke", line_index);
                            return false;
                        }
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "illum ")) {
                        //TODO: Implement MtlReader illum modes.
                        using underlying_t = std::underlying_type_t<IlluminationModel>;
                        m_illuminationModel = static_cast<IlluminationModel>(static_cast<underlying_t>(GetSingleValue<int>(cur_line, std::size_t{6u})));
                        DebuggerPrintf("MtlReader: illum modes not supported. Yet.\n");
                        PrintErrorToDebugger(filepath, "illum", line_index);
                        continue;
                    }
                }
                return true;
            }
        }
        return false;
    }

    std::vector<Material*> MtlReader::GetMaterials() noexcept {
        return std::move(m_materials);
    }

    [[nodiscard]] bool GetColorEntry(Rgba& color, const std::string& cur_line, std::string_view element_id, std::filesystem::path filepath, bool already_seen, unsigned long long last_seen_on_line, unsigned long long line_index) {
        if(already_seen) {
            DebuggerPrintf("%s redefinition: Previously defined on %ull.\n", element_id.data(), last_seen_on_line);
            PrintErrorToDebugger(filepath, element_id, line_index);
            return false;
        }
        if(auto value = GetColor(cur_line); value.has_value()) {
            color = value.value();
            return true;
        } else {
            PrintErrorToDebugger(filepath, element_id, line_index);
            return false;
        }
    }

    [[nodiscard]] std::optional<Rgba> GetColor(const std::string& cur_line) noexcept {
        std::stringstream K_ss{};
        const auto color_statements = StringUtils::Split(cur_line.substr(3), ' ');
        switch(color_statements.size()) {
        case 4: {
            if(auto xyz_str = color_statements[0]; xyz_str == "xyz") {
                const auto convert_xyz_sRGB = [&](float x, float y, float z) -> Rgba {
                    //See https://en.wikipedia.org/wiki/SRGB#Specification_of_the_transformation
                    //for explanation of values.
                    Matrix4 transform{};
                    transform.SetXComponents(Vector4{+3.2406f, -1.5372f, -0.4986f, 0.0f});
                    transform.SetYComponents(Vector4{-0.9689f, +1.8758f, +0.0415f, 0.0f});
                    transform.SetZComponents(Vector4{+0.0557f, -0.2040f, +1.0570f, 0.0f});
                    Vector3 xyz_values{x, y, z};
                    auto vrgba = Vector4(transform.TransformVector(xyz_values), 1.0f);
                    Rgba result{};
                    result.SetFromFloats({vrgba.x, vrgba.y, vrgba.z, vrgba.w});
                    return result;
                };
                return std::make_optional(convert_xyz_sRGB(std::stof(color_statements[1]), std::stof(color_statements[2]), std::stof(color_statements[3])));
            } else {
                DebuggerPrintf("MtlReader: Invalid number of arguments.\n");
                return {};
            }
        }
        case 3: {
            if(auto is_spectral_file = color_statements[0] == "spectral") {
                DebuggerPrintf("MtlReader does not support spectral files. Yet.\n");
                return {};
            }
            K_ss << cur_line.substr(3);
            K_ss.clear();
            K_ss.seekg(0);
            K_ss.seekp(0);
            float r{};
            float g{};
            float b{};
            K_ss >> r >> g >> b;
            return std::make_optional(Rgba{r, g, b, 1.0f});
        }
        case 2: {
            K_ss << cur_line.substr(3);
            K_ss.clear();
            K_ss.seekg(0);
            K_ss.seekp(0);
            float r{};
            float g{};
            K_ss >> r >> g;
            return std::make_optional(Rgba{r, g, 0.0f, 1.0f});
        }
        case 1: {
            K_ss << cur_line.substr(3);
            K_ss.clear();
            K_ss.seekg(0);
            K_ss.seekp(0);
            float r{};
            K_ss >> r;
            return std::make_optional(Rgba{r, r, r, 1.0f});
        }
        default: {
            DebuggerPrintf("Ill-formed Mtl file.\n");
            return {};
        }
        }
    }

    std::filesystem::path GetTexturePath(const std::filesystem::path& filepath, const std::string& cur_line) noexcept {
        std::stringstream ss{};
        ss << cur_line.substr(7);
        ss.clear();
        ss.seekg(0);
        ss.seekp(0);
        auto path = std::filesystem::path{ss.str()};
        if(path.is_relative()) {
            path = std::filesystem::canonical(filepath.parent_path() / path);
        }
        path.make_preferred();
        return path;
    }

    void PrintErrorToDebugger(std::filesystem::path& filepath, std::string_view elementType, unsigned long long line_index) noexcept {
        namespace FS = std::filesystem;
        filepath = FS::canonical(filepath);
        filepath.make_preferred();
        DebuggerPrintf("%s(%lld): Invalid %s\n", filepath.string().c_str(), line_index, elementType.data());
    }

} // namespace FileUtils
