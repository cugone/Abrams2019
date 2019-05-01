#include "Engine/Renderer/BlendState.hpp"

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDevice.hpp"

#include <algorithm>
#include <locale>
#include <string>

bool BlendState::CreateBlendState(const RHIDevice* device, BlendDesc render_target /*= BlendDesc()*/) {
    const std::vector<BlendDesc> targets{ 1, render_target };
    return CreateBlendState(device, targets);
}

bool BlendState::CreateBlendState(const RHIDevice* device, const std::vector<BlendDesc>& render_targets /*= {BlendDesc()}*/) {
    D3D11_BLEND_DESC desc{};
    desc.AlphaToCoverageEnable = this->_alpha_to_coverage_enable;
    desc.IndependentBlendEnable = this->_independant_blend_enable;

    const auto targets_count = render_targets.size();
    for(auto i = 0u; i < 8u && i < targets_count; ++i) {
        desc.RenderTarget[i].BlendEnable = render_targets[i].enable;
        desc.RenderTarget[i].BlendOp = BlendOpToD3DBlendOp(render_targets[i].blend_op);
        desc.RenderTarget[i].SrcBlend = BlendFactorToD3DBlendFactor(render_targets[i].source_factor);
        desc.RenderTarget[i].DestBlend = BlendFactorToD3DBlendFactor(render_targets[i].dest_factor);

        desc.RenderTarget[i].BlendOpAlpha = BlendOpToD3DBlendOp(render_targets[i].blend_op_alpha);
        desc.RenderTarget[i].SrcBlendAlpha = BlendFactorToD3DBlendFactor(render_targets[i].source_factor_alpha);
        desc.RenderTarget[i].DestBlendAlpha = BlendFactorToD3DBlendFactor(render_targets[i].dest_factor_alpha);

        desc.RenderTarget[i].RenderTargetWriteMask = BlendColorWriteEnableToD3DBlendColorWriteEnable(render_targets[i].blend_color_write_enable);
    }
    HRESULT hr = device->GetDxDevice()->CreateBlendState(&desc, &_dx_state);
    return SUCCEEDED(hr);
}

BlendState::BlendState(const RHIDevice* device, const XMLElement& element) {
    if(auto xml_blends = element.FirstChildElement("blends")) {
        DataUtils::ValidateXmlElement(*xml_blends, "blends", "blend", "", "", "alphacoverage,independantblend");
        this->_alpha_to_coverage_enable = DataUtils::ParseXmlAttribute(element, "alphacoverage", this->_alpha_to_coverage_enable);
        this->_independant_blend_enable = DataUtils::ParseXmlAttribute(element, "independantblend", this->_independant_blend_enable);

        DataUtils::ForEachChildElement(*xml_blends, "blend",
                                           [this](const XMLElement& element) {
            _descs.push_back(BlendDesc{ element });
        });

        if(!CreateBlendState(device, _descs)) {
            if(_dx_state) {
                _dx_state->Release();
                _dx_state = nullptr;
            }
            ERROR_AND_DIE("\nBlendState: Failed to create.\n");
        }
    }
}

BlendState::BlendState(const RHIDevice* device, const BlendDesc& desc /*= BlendDesc{}*/, bool alphaCoverage /*= false*/)
    : BlendState(device, std::vector<BlendDesc>{1, desc}, alphaCoverage, false)
{
    /* DO NOTHING */
}

BlendState::BlendState(const RHIDevice* device, const std::vector<BlendDesc>& descs /*= std::vector<BlendDesc>{}*/, bool alphaCoverage /*= false*/, bool independantBlend /*= false*/)
    : _alpha_to_coverage_enable(alphaCoverage)
    , _independant_blend_enable(independantBlend)
    , _descs{ descs }
{
    if(!CreateBlendState(device, _descs)) {
        if(_dx_state) {
            _dx_state->Release();
            _dx_state = nullptr;
        }
        ERROR_AND_DIE("\nBlendState: Failed to create.\n");
    }
}

BlendState::~BlendState() {
    if(_dx_state) {
        _dx_state->Release();
        _dx_state = nullptr;
    }
}

ID3D11BlendState* BlendState::GetDxBlendState() {
    return _dx_state;
}

BlendDesc::BlendDesc(const XMLElement& element) {
    DataUtils::ValidateXmlElement(element, "blend", "", "", "color,alpha,enablemask", "enable");
    this->enable = DataUtils::ParseXmlAttribute(element, "enable", this->enable);

    if(auto xml_color = element.FirstChildElement("color")) {
        DataUtils::ValidateXmlElement(*xml_color, "color", "", "src,dest,op");
        std::string source_factor_str = "one";
        source_factor_str = DataUtils::ParseXmlAttribute(*xml_color, "src", source_factor_str);
        std::string dest_factor_str = "zero";
        dest_factor_str = DataUtils::ParseXmlAttribute(*xml_color, "dest", dest_factor_str);
        std::string op_str = "add";
        op_str = DataUtils::ParseXmlAttribute(*xml_color, "op", op_str);

        this->source_factor = BlendFactorFromString(source_factor_str);
        this->dest_factor = BlendFactorFromString(dest_factor_str);
        this->blend_op = BlendOperationFromString(op_str);
    }

    if(auto xml_alpha = element.FirstChildElement("alpha")) {
        DataUtils::ValidateXmlElement(*xml_alpha, "alpha", "", "src,dest,op");
        std::string source_factor_str = "one";
        source_factor_str = DataUtils::ParseXmlAttribute(*xml_alpha, "src", source_factor_str);
        std::string dest_factor_str = "zero";
        dest_factor_str = DataUtils::ParseXmlAttribute(*xml_alpha, "dest", dest_factor_str);
        std::string op_str = "add";
        op_str = DataUtils::ParseXmlAttribute(*xml_alpha, "op", op_str);

        this->source_factor_alpha = BlendFactorFromString(source_factor_str);
        this->dest_factor_alpha = BlendFactorFromString(dest_factor_str);
        this->blend_op_alpha = BlendOperationFromString(op_str);
    }

    if(auto xml_mask = element.FirstChildElement("enablemask")) {
        DataUtils::ValidateXmlElement(*xml_mask, "enablemask", "", "value");
        std::string mask_str = "all";
        mask_str = DataUtils::ParseXmlAttribute(*xml_mask, "value", mask_str);

        this->blend_color_write_enable = BlendColorWriteEnableFromString(mask_str);
    }
}
