#include "Engine/Renderer/Material.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Win.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>

Material::Material(Renderer& renderer) noexcept
: _renderer(renderer)
, _textures(CustomTextureIndexSlotOffset, nullptr) {
    _textures[0] = _renderer.GetTexture("__diffuse");
    _textures[1] = _renderer.GetTexture("__normal");
    _textures[2] = _renderer.GetTexture("__displacement");
    _textures[3] = _renderer.GetTexture("__specular");
    _textures[4] = _renderer.GetTexture("__occlusion");
    _textures[5] = _renderer.GetTexture("__emissive");

    std::size_t count = _renderer.GetMaterialCount();
    _name += "_" + std::to_string(count);
}

Material::Material(Renderer& renderer, const XMLElement& element) noexcept
: _renderer(renderer)
, _textures(CustomTextureIndexSlotOffset, nullptr) {
    _textures[0] = _renderer.GetTexture("__diffuse");
    _textures[1] = _renderer.GetTexture("__normal");
    _textures[2] = _renderer.GetTexture("__displacement");
    _textures[3] = _renderer.GetTexture("__specular");
    _textures[4] = _renderer.GetTexture("__occlusion");
    _textures[5] = _renderer.GetTexture("__emissive");

    std::size_t count = _renderer.GetMaterialCount();
    _name += "_" + std::to_string(count);

    GUARANTEE_OR_DIE(LoadFromXml(element), "Material constructor failed to load.");
}

bool Material::LoadFromXml(const XMLElement& element) noexcept {
    namespace FS = std::filesystem;

    DataUtils::ValidateXmlElement(element, "material", "shader", "name", "lighting,textures");

    _name = DataUtils::ParseXmlAttribute(element, "name", _name);

    {
        const auto xml_shader = element.FirstChildElement("shader");
        DataUtils::ValidateXmlElement(*xml_shader, "shader", "", "src");
        const auto file = DataUtils::ParseXmlAttribute(*xml_shader, "src", "");
        FS::path shader_src(file);
        if(!StringUtils::StartsWith(shader_src.string(), "__")) {
            std::error_code ec{};
            shader_src = FS::canonical(shader_src, ec);
            const auto error_msg = [&]() {
                std::ostringstream ss;
                ss << "Shader:\n";
                ss << file << "\n";
                ss << "Referenced in Material file \"" << _name << "\" could not be found.\n";
                ss << "The filesystem returned an error:\n"
                   << ec.message() << '\n';
                return ss.str();
            }(); //IIIL
            GUARANTEE_OR_DIE(!ec, error_msg.c_str());
        }
        shader_src.make_preferred();
        if(auto shader = _renderer.GetShader(shader_src.string())) {
            _shader = shader;
        } else {
            DebuggerPrintf("Shader: %s\n referenced in Material file \"%s\" did not already exist. Attempting to create from source...", shader_src.string().c_str(), _name.c_str());
            if(!_renderer.RegisterShader(shader_src.string())) {
                DebuggerPrintf("failed.\n");
                return false;
            }
            DebuggerPrintf("done.\n");
            if(shader = _renderer.GetShader(shader_src.string()); shader == nullptr) {
                if(shader = _renderer.GetShader(_renderer.GetShaderName(shader_src)); shader != nullptr) {
                    _shader = shader;
                }
            }
        }
    }

    if(const auto xml_lighting = element.FirstChildElement("lighting")) {
        DataUtils::ValidateXmlElement(*xml_lighting, "lighting", "", "", "specularIntensity,specularFactor,specularPower,glossFactor,emissiveFactor");
        //specularIntensity and specularFactor are synonyms
        if(const auto xml_specInt = xml_lighting->FirstChildElement("specularIntensity")) {
            _specularIntensity = DataUtils::ParseXmlElementText(*xml_specInt, _specularIntensity);
        }
        if(const auto xml_specFactor = xml_lighting->FirstChildElement("specularFactor")) {
            _specularIntensity = DataUtils::ParseXmlElementText(*xml_specFactor, _specularIntensity);
        }
        //specularPower and glossFactor are synonyms
        if(const auto xml_specPower = xml_lighting->FirstChildElement("specularPower")) {
            _specularPower = DataUtils::ParseXmlElementText(*xml_specPower, _specularPower);
        }
        if(const auto xml_glossFactor = xml_lighting->FirstChildElement("glossFactor")) {
            _specularPower = DataUtils::ParseXmlElementText(*xml_glossFactor, _specularPower);
        }
        if(const auto xml_emissiveFactor = xml_lighting->FirstChildElement("emissiveFactor")) {
            _emissiveFactor = DataUtils::ParseXmlElementText(*xml_emissiveFactor, _emissiveFactor);
        }
    }

    if(const auto xml_textures = element.FirstChildElement("textures")) {
        const auto invalid_tex = _renderer.GetTexture("__invalid");

        if(const auto xml_diffuse = xml_textures->FirstChildElement("diffuse")) {
            const auto file = DataUtils::ParseXmlAttribute(*xml_diffuse, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    bad_path = true;
                    _textures[0] = invalid_tex;
                    DebuggerPrintf("Diffuse texture referenced in Material file \"%s\" could not be found. The filesystem returned an error: %s\n", _name.c_str(), ec.message().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_loaded = _renderer.IsTextureNotLoaded(p_str);
                if(texture_not_loaded) {
                    texture_not_loaded = _renderer.CreateTexture(p.string(), IntVector3::XY_AXIS) ? false : true;
                }
                bool texture_not_exist = !empty_path && texture_not_loaded;
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[0] = tex;
            }
        }

        if(const auto xml_normal = xml_textures->FirstChildElement("normal")) {
            const auto file = DataUtils::ParseXmlAttribute(*xml_normal, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    bad_path = true;
                    _textures[1] = invalid_tex;
                    DebuggerPrintf("Normal texture referenced in Material file \"%s\" could not be found. The filesystem returned an error: %s\n", _name.c_str(), ec.message().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_loaded = _renderer.IsTextureNotLoaded(p_str);
                if(texture_not_loaded) {
                    texture_not_loaded = _renderer.CreateTexture(p.string(), IntVector3::XY_AXIS) ? false : true;
                }
                bool texture_not_exist = !empty_path && texture_not_loaded;
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[1] = tex;
            }
        }

        if(const auto xml_displacement = xml_textures->FirstChildElement("displacement")) {
            const auto file = DataUtils::ParseXmlAttribute(*xml_displacement, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    bad_path = true;
                    _textures[2] = invalid_tex;
                    DebuggerPrintf("Displacement texture referenced in Material file \"%s\" could not be found. The filesystem returned an error: %s\n", _name.c_str(), ec.message().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_loaded = _renderer.IsTextureNotLoaded(p_str);
                if(texture_not_loaded) {
                    texture_not_loaded = _renderer.CreateTexture(p.string(), IntVector3::XY_AXIS) ? false : true;
                }
                bool texture_not_exist = !empty_path && texture_not_loaded;
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[2] = tex;
            }
        }

        if(const auto xml_specular = xml_textures->FirstChildElement("specular")) {
            const auto file = DataUtils::ParseXmlAttribute(*xml_specular, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    bad_path = true;
                    _textures[3] = invalid_tex;
                    DebuggerPrintf("Specular texture referenced in Material file \"%s\" could not be found. The filesystem returned an error: %s\n", _name.c_str(), ec.message().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_loaded = _renderer.IsTextureNotLoaded(p_str);
                if(texture_not_loaded) {
                    texture_not_loaded = _renderer.CreateTexture(p.string(), IntVector3::XY_AXIS) ? false : true;
                }
                bool texture_not_exist = !empty_path && texture_not_loaded;
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[3] = tex;
            }
        }

        if(const auto xml_occlusion = xml_textures->FirstChildElement("occlusion")) {
            const auto file = DataUtils::ParseXmlAttribute(*xml_occlusion, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    bad_path = true;
                    _textures[4] = invalid_tex;
                    DebuggerPrintf("Occlusion texture referenced in Material file \"%s\" could not be found. The filesystem returned an error: %s\n", _name.c_str(), ec.message().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_loaded = _renderer.IsTextureNotLoaded(p_str);
                if(texture_not_loaded) {
                    texture_not_loaded = _renderer.CreateTexture(p.string(), IntVector3::XY_AXIS) ? false : true;
                }
                bool texture_not_exist = !empty_path && texture_not_loaded;
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[4] = tex;
            }
        }

        if(const auto xml_emissive = xml_textures->FirstChildElement("emissive")) {
            const auto file = DataUtils::ParseXmlAttribute(*xml_emissive, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    bad_path = true;
                    _textures[5] = invalid_tex;
                    DebuggerPrintf("Emissive texture referenced in Material file \"%s\" could not be found. The filesystem returned an error: %s\n", _name.c_str(), ec.message().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_loaded = _renderer.IsTextureNotLoaded(p_str);
                if(texture_not_loaded) {
                    texture_not_loaded = _renderer.CreateTexture(p.string(), IntVector3::XY_AXIS) ? false : true;
                }
                bool texture_not_exist = !empty_path && texture_not_loaded;
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[5] = tex;
            }
        }
        {
            const auto numTextures = DataUtils::GetChildElementCount(*xml_textures, "texture");
            if(numTextures >= MaxCustomTextureSlotCount) {
                DebuggerPrintf("Max custom texture count exceeded. Cannot bind more than %i custom textures.", MaxCustomTextureSlotCount);
            }
            AddTextureSlots(numTextures);
        }

        DataUtils::ForEachChildElement(*xml_textures, "texture",
                                       [this, &invalid_tex](const XMLElement& elem) {
                                           DataUtils::ValidateXmlElement(elem, "texture", "", "index,src");
                                           std::size_t index = CustomTextureIndexSlotOffset + DataUtils::ParseXmlAttribute(elem, std::string("index"), 0u);
                                           if(index >= CustomTextureIndexSlotOffset + MaxCustomTextureSlotCount) {
                                               return;
                                           }
                                           auto file = DataUtils::ParseXmlAttribute(elem, "src", "");
                                           FS::path p(file);
                                           bool bad_path = false;
                                           if(!StringUtils::StartsWith(p.string(), "__")) {
                                               std::error_code ec{};
                                               p = FS::canonical(p, ec);
                                               if(ec) {
                                                   bad_path = true;
                                                   _textures[index] = invalid_tex;
                                                   DebuggerPrintf("Custom texture at index %lu referenced in Material file \"%s\" could not be found. The filesystem returned an error: %s\n", index, _name.c_str(), ec.message().c_str());
                                               }
                                           }
                                           if(!bad_path) {
                                               p.make_preferred();
                                               const auto& p_str = p.string();
                                               bool empty_path = p.empty();
                                               bool texture_not_loaded = _renderer.IsTextureNotLoaded(p_str);
                                               if(texture_not_loaded) {
                                                   texture_not_loaded = _renderer.CreateTexture(p.string(), IntVector3::XY_AXIS) ? false : true;
                                               }
                                               bool texture_not_exist = !empty_path && texture_not_loaded;
                                               bool invalid_src = empty_path || texture_not_exist;
                                               auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                                               _textures[index] = tex;
                                           }
                                       });
    }
    return true;
}

void Material::AddTextureSlots(std::size_t count) noexcept {
    const auto old_size = _textures.size();
    const auto new_size = (std::min)(old_size + MaxCustomTextureSlotCount, old_size + (std::min)(MaxCustomTextureSlotCount, count));
    _textures.resize(new_size);
    for(std::size_t i = old_size; i < new_size; ++i) {
        _textures[i] = nullptr;
    }
}

void Material::AddTextureSlot() noexcept {
    AddTextureSlots(1);
}

std::string Material::GetName() const noexcept {
    return _name;
}

Shader* Material::GetShader() const noexcept {
    return _shader;
}

std::size_t Material::GetTextureCount() const noexcept {
    return _textures.size();
}

Texture* Material::GetTexture(std::size_t i) const noexcept {
    return _textures[i];
}

Texture* Material::GetTexture(const TextureID& id) const noexcept {
    return GetTexture(std::underlying_type_t<TextureID>(id));
}

float Material::GetSpecularIntensity() const noexcept {
    return _specularIntensity;
}

float Material::GetGlossyFactor() const noexcept {
    return _specularPower;
}

float Material::GetEmissiveFactor() const noexcept {
    return _emissiveFactor;
}

Vector3 Material::GetSpecGlossEmitFactors() const noexcept {
    return Vector3(GetSpecularIntensity(), GetGlossyFactor(), GetEmissiveFactor());
}

void Material::SetFilepath(const std::filesystem::path& p) noexcept {
    _filepath = p;
}

const std::filesystem::path& Material::GetFilepath() const noexcept {
    return _filepath;
}

void Material::SetTextureSlot(const TextureID& slotId, Texture* texture) noexcept {
    using underlying = std::underlying_type_t<TextureID>;
    const auto slotAsIndex = static_cast<underlying>(slotId);
    const auto count = GetTextureCount();
    if(count <= slotAsIndex) {
        AddTextureSlots(1 + count - slotAsIndex);
    }
    _textures[slotAsIndex] = texture;
}
