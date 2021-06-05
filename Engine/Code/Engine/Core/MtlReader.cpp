#include "Engine/Core/MtlReader.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Profiling/ProfileLogScope.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Material.hpp"

#include <numeric>
#include <sstream>

namespace FileUtils {

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
                unsigned long long line_index = 0;
                while(std::getline(ss, cur_line, '\n')) {
                    ++line_index;
                    cur_line = cur_line.substr(0, cur_line.find_first_of('#'));
                    if(cur_line.empty()) {
                        continue;
                    }
                    cur_line = StringUtils::TrimWhitespace(cur_line);
                    if(StringUtils::StartsWith(cur_line, "newmtl ")) {
                        auto material = std::make_unique<Material>(_renderer);
                        material->_name = cur_line.substr(7);
                        _renderer.RegisterMaterial(std::move(material));
                        m_materials.emplace_back(material.get());
                        continue;
                    }
                    if(StringUtils::StartsWith(cur_line, "Ns ")) {
                        std::stringstream Ns_ss{};
                        Ns_ss << cur_line.substr(3);
                        Ns_ss.clear();
                        Ns_ss.seekg(0);
                        Ns_ss.seekp(0);
                        Ns_ss >> m_materials.back()->_specularPower;
                    }
                    if(StringUtils::StartsWith(cur_line, "Ks ")) {
                        std::stringstream Ks_ss{};
                        Ks_ss << cur_line.substr(3);
                        Ks_ss.clear();
                        Ks_ss.seekg(0);
                        Ks_ss.seekp(0);
                        Rgba specColor{};
                        Ks_ss >> m_materials.back()->_specularIntensity;
                    }
                    if(StringUtils::StartsWith(cur_line, "map_Kd ")) {
                        std::stringstream mapKd_ss{};
                        mapKd_ss << cur_line.substr(7);
                        mapKd_ss.clear();
                        mapKd_ss.seekg(0);
                        mapKd_ss.seekp(0);
                        Rgba specColor{};
                        auto* diffuse = _renderer.GetTexture(mapKd_ss.str());
                        m_materials.back()->SetTextureSlot(Material::TextureID::Diffuse, diffuse);
                    }
                    /*
                        * # Blender MTL File: 'lemur.blend'
                        * # Material Count: 1
                        * newmtl LemurMat
                        * Ns 96.078431
                        * Ka 1.000000 1.000000 1.000000
                        * Kd 1.000000 1.000000 1.000000
                        * Ks 0.000000 0.000000 0.000000
                        * Ke 0.000000 0.000000 0.000000
                        * Ni 1.000000
                        * d 1.000000
                        * illum 1
                        * map_Kd lemurT.png
                    */
                }
                return true;
            }
        }
        return false;
    }

    std::vector<Material*> MtlReader::GetMaterials() noexcept {
        return std::move(m_materials);
    }

    void MtlReader::PrintErrorToDebugger(std::filesystem::path filepath, const std::string& elementType, unsigned long long line_index) const noexcept {
        namespace FS = std::filesystem;
        filepath = FS::canonical(filepath);
        filepath.make_preferred();
        DebuggerPrintf("%s(%lld): Invalid %s\n", filepath.string().c_str(), line_index, elementType.c_str());
    }

} // namespace FileUtils
