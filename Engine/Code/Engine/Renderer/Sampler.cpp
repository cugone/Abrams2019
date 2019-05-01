#include "Engine/Renderer/Sampler.hpp"

#include "Engine/Core/BuildConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"

void Sampler::SetDebugName([[maybe_unused]] const std::string& name) const noexcept {
#ifdef RENDER_DEBUG
    _dx_state->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned int>(name.size()), name.data());
#endif
}

Sampler::Sampler(const RHIDevice* device, const XMLElement& element)
    : Sampler(device, SamplerDesc{ element }) {
    /* DO NOTHING */
}

Sampler::Sampler(const RHIDevice* device, const SamplerDesc& desc) {
    if(!CreateSamplerState(device, desc)) {
        if(_dx_state) {
            _dx_state->Release();
            _dx_state = nullptr;
        }
        ERROR_AND_DIE("Sampler: dx Sample failed to create.\n");
    }
}

Sampler::~Sampler() {
    if(_dx_state) {
        _dx_state->Release();
        _dx_state = nullptr;
    }
}

ID3D11SamplerState* Sampler::GetDxSampler() const {
    return _dx_state;
}

bool Sampler::CreateSamplerState(const RHIDevice* device, const SamplerDesc& desc /*= SamplerDesc()*/) {

    D3D11_SAMPLER_DESC dx_desc{};

    dx_desc.Filter = FilterModeToD3DFilter(desc.min_filter, desc.mag_filter, desc.mip_filter, desc.compare_mode);

    dx_desc.AddressU = AddressModeToD3DAddressMode(desc.UaddressMode);
    dx_desc.AddressV = AddressModeToD3DAddressMode(desc.VaddressMode);
    dx_desc.AddressW = AddressModeToD3DAddressMode(desc.WaddressMode);

    dx_desc.MinLOD = desc.minLOD;
    dx_desc.MaxLOD = desc.maxLOD;

    dx_desc.MipLODBias = desc.mipmapLODBias;

    dx_desc.MaxAnisotropy = desc.maxAnisotropicLevel;

    dx_desc.ComparisonFunc = ComparisonFunctionToD3DComparisonFunction(desc.compareFunc);

    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
    desc.borderColor.GetAsFloats(r, g, b, a);
    dx_desc.BorderColor[0] = r;
    dx_desc.BorderColor[1] = g;
    dx_desc.BorderColor[2] = b;
    dx_desc.BorderColor[3] = a;

    HRESULT hr = device->GetDxDevice()->CreateSamplerState(&dx_desc, &_dx_state);
    return SUCCEEDED(hr);
}

SamplerDesc::SamplerDesc(const XMLElement& element) {
    if(auto xml_sampler = element.FirstChildElement("sampler")) {
        DataUtils::ValidateXmlElement(*xml_sampler, "sampler", "", "", "filter,textureAddress,lod", "borderColor,test,maxAF");

        this->borderColor = DataUtils::ParseXmlAttribute(*xml_sampler, "borderColor", this->borderColor);

        std::string compare_str = "never";
        compare_str = DataUtils::ParseXmlAttribute(*xml_sampler, "test", compare_str);
        this->compareFunc = ComparisonFunctionFromString(compare_str);

        this->maxAnisotropicLevel = DataUtils::ParseXmlAttribute(*xml_sampler, "maxAF", this->maxAnisotropicLevel);

        if(auto xml_filter = xml_sampler->FirstChildElement("filter")) {
            DataUtils::ValidateXmlElement(*xml_filter, "filter", "", "min,mag,mip,mode");

            std::string filter_str = "point";
            filter_str = DataUtils::ParseXmlAttribute(*xml_filter, "min", filter_str);
            this->min_filter = FilterModeFromString(filter_str);

            filter_str = "point";
            filter_str = DataUtils::ParseXmlAttribute(*xml_filter, "mag", filter_str);
            this->mag_filter = FilterModeFromString(filter_str);

            filter_str = "point";
            filter_str = DataUtils::ParseXmlAttribute(*xml_filter, "mip", filter_str);
            this->mip_filter = FilterModeFromString(filter_str);

            compare_str = "none";
            compare_str = DataUtils::ParseXmlAttribute(*xml_filter, "mode", compare_str);
            this->compare_mode = FilterComparisonModeFromString(compare_str);
        }
        if(auto xml_textureAddress = xml_sampler->FirstChildElement("textureAddress")) {

            DataUtils::ValidateXmlElement(*xml_textureAddress, "textureAddress", "", "", "", "u,v,w");

            std::string str = "wrap";
            str = DataUtils::ParseXmlAttribute(*xml_textureAddress, "u", str);
            this->UaddressMode = TextureAddressModeFromString(str);

            str = "wrap";
            str = DataUtils::ParseXmlAttribute(*xml_textureAddress, "v", str);
            this->VaddressMode = TextureAddressModeFromString(str);

            str = "wrap";
            str = DataUtils::ParseXmlAttribute(*xml_textureAddress, "w", str);
            this->WaddressMode = TextureAddressModeFromString(str);

        }
        if(auto xml_lod = xml_sampler->FirstChildElement("lod")) {
            DataUtils::ValidateXmlElement(*xml_lod, "lod", "", "", "", "min,max,mipmapbias");
            this->minLOD = DataUtils::ParseXmlAttribute(*xml_lod, "min", this->minLOD);
            this->maxLOD = DataUtils::ParseXmlAttribute(*xml_lod, "max", this->maxLOD);
            this->mipmapLODBias = DataUtils::ParseXmlAttribute(*xml_lod, "mipmapbias", this->mipmapLODBias);
        }
    }

}
