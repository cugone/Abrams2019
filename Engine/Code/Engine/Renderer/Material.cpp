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
    , _textures(CustomTextureIndexSlotOffset, nullptr)
{
    _textures[0] = _renderer.GetTexture("__diffuse");
    _textures[1] = _renderer.GetTexture("__normal");
    _textures[2] = _renderer.GetTexture("__displacement");
    _textures[3] = _renderer.GetTexture("__specular");
    _textures[4] = _renderer.GetTexture("__occlusion");
    _textures[5] = _renderer.GetTexture("__emissive");

    std::size_t count = _renderer.GetMaterialCount();
    std::ostringstream ss;
    ss << '_' << count;
    _name += ss.str();
}

Material::Material(Renderer& renderer, const XMLElement& element) noexcept
    : _renderer(renderer)
    , _textures(CustomTextureIndexSlotOffset, nullptr)
{
    _textures[0] = _renderer.GetTexture("__diffuse");
    _textures[1] = _renderer.GetTexture("__normal");
    _textures[2] = _renderer.GetTexture("__displacement");
    _textures[3] = _renderer.GetTexture("__specular");
    _textures[4] = _renderer.GetTexture("__occlusion");
    _textures[5] = _renderer.GetTexture("__emissive");

    std::size_t count = _renderer.GetMaterialCount();
    std::ostringstream ss;
    ss << '_' << count;
    _name += ss.str();

    LoadFromXml(element);
}

bool Material::LoadFromXml(const XMLElement& element) noexcept {
    namespace FS = std::filesystem;

    DataUtils::ValidateXmlElement(element, "material", "shader", "name", "lighting,textures");

    _name = DataUtils::ParseXmlAttribute(element, "name", _name);

    {
        auto xml_shader = element.FirstChildElement("shader");
        DataUtils::ValidateXmlElement(*xml_shader, "shader", "", "src");
        auto file = DataUtils::ParseXmlAttribute(*xml_shader, "src", "");
        FS::path p(file);
        if(!StringUtils::StartsWith(p.string(), "__")) {
            std::error_code ec{};
            p = FS::canonical(p, ec);
            if(ec) {
                std::ostringstream ss;
                ss << "Shader:\n";
                ss << file << "\n";
                ss << "Referenced in Material file \"" << _name << "\" could not be found.\n";
                ss << "The filesystem returned an error:\n" << ec.message() << '\n';
                ERROR_AND_DIE(ss.str().c_str());
                return false;
            }
        }
        p.make_preferred();
        if(auto shader = _renderer.GetShader(p.string())) {
            _shader = shader;
        } else {
            std::ostringstream ss;
            ss << "Shader: " << p.string() << "\n referenced in Material file \"" << _name << "\" did not already exist. Attempting to create from source...";
            DebuggerPrintf(ss.str().c_str());
            ss.str("");
            if(!_renderer.RegisterShader(p.string())) {
                ss << "failed.\n";
                DebuggerPrintf(ss.str().c_str());
                return false;
            }
            ss << "done.\n";
            DebuggerPrintf(ss.str().c_str());
            _shader = _renderer.GetShader(p.string());
        }
    }

    if(auto xml_lighting = element.FirstChildElement("lighting")) {
        DataUtils::ValidateXmlElement(*xml_lighting, "lighting", "", "", "specularIntensity,specularFactor,specularPower,glossFactor,emissiveFactor");
        //specularIntensity and specularFactor are synonyms
        if(auto xml_specInt = xml_lighting->FirstChildElement("specularIntensity")) {
            _specularIntensity = DataUtils::ParseXmlElementText(*xml_specInt, _specularIntensity);
        }
        if(auto xml_specFactor = xml_lighting->FirstChildElement("specularFactor")) {
            _specularIntensity = DataUtils::ParseXmlElementText(*xml_specFactor, _specularIntensity);
        }
        //specularPower and glossFactor are synonyms
        if(auto xml_specPower = xml_lighting->FirstChildElement("specularPower")) {
            _specularPower = DataUtils::ParseXmlElementText(*xml_specPower, _specularPower);
        }
        if(auto xml_glossFactor = xml_lighting->FirstChildElement("glossFactor")) {
            _specularPower = DataUtils::ParseXmlElementText(*xml_glossFactor, _specularPower);
        }
        if(auto xml_emissiveFactor = xml_lighting->FirstChildElement("emissiveFactor")) {
            _emissiveFactor = DataUtils::ParseXmlElementText(*xml_emissiveFactor, _emissiveFactor);
        }
    }

    if(auto xml_textures = element.FirstChildElement("textures")) {
        auto invalid_tex = _renderer.GetTexture("__invalid");

        if(auto xml_diffuse = xml_textures->FirstChildElement("diffuse")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_diffuse, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    std::ostringstream ss;
                    ss << "Diffuse texture referenced in Material file \"" << _name << "\" could not be found.";
                    ss << "The filesystem returned an error: " << ec.message() << '\n';
                    bad_path = true;
                    _textures[0] = invalid_tex;
                    DebuggerPrintf(ss.str().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_loaded = _renderer.IsTextureNotLoaded(p_str);
                if(texture_not_loaded) {
                    _renderer.CreateTexture(p.string(), IntVector3::XY_AXIS);
                    texture_not_loaded = _renderer.IsTextureNotLoaded(p_str);
                }
                bool texture_not_exist = !empty_path && texture_not_loaded;
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[0] = tex;
            }
        }

        if(auto xml_normal = xml_textures->FirstChildElement("normal")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_normal, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    std::ostringstream ss;
                    ss << "Normal texture referenced in Material file \"" << _name << "\" could not be found.";
                    ss << "The filesystem returned an error: " << ec.message() << '\n';
                    bad_path = true;
                    _textures[1] = invalid_tex;
                    DebuggerPrintf(ss.str().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_exist = !empty_path && _renderer.IsTextureNotLoaded(p_str);
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[1] = tex;
            }
        }

        if(auto xml_displacement = xml_textures->FirstChildElement("displacement")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_displacement, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    std::ostringstream ss;
                    ss << "Displacement texture referenced in Material file \"" << _name << "\" could not be found.";
                    ss << "The filesystem returned an error: " << ec.message() << '\n';
                    _textures[2] = invalid_tex;
                    bad_path = true;
                    DebuggerPrintf(ss.str().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_exist = !empty_path && _renderer.IsTextureNotLoaded(p_str);
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[2] = tex;
            }
        }

        if(auto xml_specular = xml_textures->FirstChildElement("specular")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_specular, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    std::ostringstream ss;
                    ss << "Specular texture referenced in Material file \"" << _name << "\" could not be found.";
                    ss << "The filesystem returned an error: " << ec.message() << '\n';
                    _textures[3] = invalid_tex;
                    bad_path = true;
                    DebuggerPrintf(ss.str().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_exist = !empty_path && _renderer.IsTextureNotLoaded(p_str);
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[3] = tex;
            }
        }

        if(auto xml_occlusion = xml_textures->FirstChildElement("occlusion")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_occlusion, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    std::ostringstream ss;
                    ss << "Occlusion texture referenced in Material file \"" << _name << "\" could not be found.";
                    ss << "The filesystem returned an error: " << ec.message() << '\n';
                    _textures[4] = invalid_tex;
                    bad_path = true;
                    DebuggerPrintf(ss.str().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_exist = !empty_path && _renderer.IsTextureNotLoaded(p_str);
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[4] = tex;
            }
        }

        if(auto xml_emissive = xml_textures->FirstChildElement("emissive")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_emissive, "src", "");
            FS::path p(file);
            bool bad_path = false;
            if(!StringUtils::StartsWith(p.string(), "__")) {
                std::error_code ec{};
                p = FS::canonical(p, ec);
                if(ec) {
                    std::ostringstream ss;
                    ss << "Emissive texture referenced in Material file \"" << _name << "\" could not be found.";
                    ss << "The filesystem returned an error: " << ec.message() << '\n';
                    _textures[5] = invalid_tex;
                    bad_path = true;
                    DebuggerPrintf(ss.str().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_exist = !empty_path && _renderer.IsTextureNotLoaded(p_str);
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[5] = tex;
            }
        }
        {
            auto numTextures = DataUtils::GetChildElementCount(*xml_textures, "texture");
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
                    std::ostringstream ss;
                    ss << "Custom texture at index " << index << " referenced in Material file \"" << _name << "\" could not be found.";
                    ss << "The filesystem returned an error: " << ec.message() << '\n';
                    bad_path = true;
                    _textures[index] = invalid_tex;
                    DebuggerPrintf(ss.str().c_str());
                }
            }
            if(!bad_path) {
                p.make_preferred();
                const auto& p_str = p.string();
                bool empty_path = p.empty();
                bool texture_not_exist = !empty_path && _renderer.IsTextureNotLoaded(p_str);
                bool invalid_src = empty_path || texture_not_exist;
                auto tex = invalid_src ? invalid_tex : (_renderer.GetTexture(p_str));
                _textures[index] = tex;
            }
        });
    }
    return true;
}

void Material::AddTextureSlots(std::size_t count) noexcept {
    std::size_t old_size = _textures.size();
    std::size_t new_size = (std::min)(old_size + MaxCustomTextureSlotCount, old_size + (std::min)(MaxCustomTextureSlotCount, count));
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

Shader * Material::GetShader() const noexcept {
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
