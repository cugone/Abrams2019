#include "Engine/Renderer/DepthStencilState.hpp"

#include "Engine/Core/BuildConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"

#include <string>

void DepthStencilState::SetDebugName([[maybe_unused]] const std::string& name) const noexcept {
#ifdef RENDER_DEBUG
    _dx_state->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned int>(name.size()), name.data());
#endif
}

DepthStencilState::DepthStencilState(const RHIDevice* device, const XMLElement& element)
: DepthStencilState(device, DepthStencilDesc{ element })
{
    /* DO NOTHING */
}

DepthStencilState::DepthStencilState(const RHIDevice* device, const DepthStencilDesc& desc)
    : _desc(desc)
{
    if(!CreateDepthStencilState(device, desc)) {
        if(_dx_state) {
            _dx_state->Release();
            _dx_state = nullptr;
        }
        ERROR_AND_DIE("DepthStencilState failed to create.");
    }
}

DepthStencilState::~DepthStencilState() {
    if(_dx_state) {
        _dx_state->Release();
        _dx_state = nullptr;
    }
}

ID3D11DepthStencilState* DepthStencilState::GetDxDepthStencilState() const {
    return _dx_state;
}

DepthStencilDesc DepthStencilState::GetDesc() const {
    return _desc;
}

bool DepthStencilState::CreateDepthStencilState(const RHIDevice* device, const DepthStencilDesc& desc /*= DepthStencilDesc{}*/) {
    D3D11_DEPTH_STENCIL_DESC dx_desc;
    dx_desc.DepthEnable = desc.depth_enabled ? TRUE : FALSE;
    dx_desc.DepthWriteMask = desc.depth_write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    dx_desc.StencilEnable = desc.stencil_enabled ? TRUE : FALSE;
    dx_desc.StencilReadMask = desc.stencil_read ? D3D11_DEFAULT_STENCIL_READ_MASK : 0x00;
    dx_desc.StencilWriteMask = desc.stencil_write ? D3D11_DEFAULT_STENCIL_WRITE_MASK : 0x00;
    dx_desc.DepthFunc = ComparisonFunctionToD3DComparisonFunction(desc.depth_comparison);

    dx_desc.FrontFace.StencilFailOp = StencilOperationToD3DStencilOperation(desc.stencil_failFrontOp);
    dx_desc.FrontFace.StencilDepthFailOp = StencilOperationToD3DStencilOperation(desc.stencil_failDepthFrontOp);
    dx_desc.FrontFace.StencilFunc = ComparisonFunctionToD3DComparisonFunction(desc.stencil_testFront);
    dx_desc.FrontFace.StencilPassOp = StencilOperationToD3DStencilOperation(desc.stencil_passFrontOp);

    dx_desc.BackFace.StencilFailOp = StencilOperationToD3DStencilOperation(desc.stencil_failBackOp);
    dx_desc.BackFace.StencilDepthFailOp = StencilOperationToD3DStencilOperation(desc.stencil_failDepthBackOp);
    dx_desc.BackFace.StencilFunc = ComparisonFunctionToD3DComparisonFunction(desc.stencil_testBack);
    dx_desc.BackFace.StencilPassOp = StencilOperationToD3DStencilOperation(desc.stencil_passBackOp);

    HRESULT hr = device->GetDxDevice()->CreateDepthStencilState(&dx_desc, &_dx_state);
    return SUCCEEDED(hr);
}

DepthStencilDesc::DepthStencilDesc(const XMLElement &element) {
    if(auto xml_depth = element.FirstChildElement("depth")) {
        DataUtils::ValidateXmlElement(*xml_depth, "depth", "", "", "", "enable,writable,test");
        this->depth_enabled = DataUtils::ParseXmlAttribute(*xml_depth, "enable", this->depth_enabled);
        this->depth_write = DataUtils::ParseXmlAttribute(*xml_depth, "writable", this->depth_write);
        std::string comp_func_str = "less";
        comp_func_str = DataUtils::ParseXmlAttribute(*xml_depth, "test", comp_func_str);
        this->depth_comparison = ComparisonFunctionFromString(comp_func_str);
    }

    if(auto xml_stencil = element.FirstChildElement("stencil")) {
        DataUtils::ValidateXmlElement(*xml_stencil, "stencil", "", "", "front,back", "enable,writable,readable");

        this->stencil_read = DataUtils::ParseXmlAttribute(*xml_stencil, "readable", this->stencil_read);
        this->stencil_write = DataUtils::ParseXmlAttribute(*xml_stencil, "writable", this->stencil_write);
        this->stencil_enabled = DataUtils::ParseXmlAttribute(*xml_stencil, "enable", this->stencil_enabled);

        if(auto xml_stencilfront = xml_stencil->FirstChildElement("front")) {
            DataUtils::ValidateXmlElement(*xml_stencilfront, "front", "", "fail,depthfail,pass,test");

            std::string failFront_str = "keep";
            failFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "fail", failFront_str);
            this->stencil_failFrontOp = StencilOperationFromString(failFront_str);

            std::string depthfailFront_str = "keep";
            depthfailFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "depthfail", depthfailFront_str);
            this->stencil_failDepthFrontOp = StencilOperationFromString(depthfailFront_str);

            std::string passFront_str = "keep";
            passFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "pass", passFront_str);
            this->stencil_passFrontOp = StencilOperationFromString(passFront_str);

            std::string compareFront_str = "always";
            compareFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "test", compareFront_str);
            this->stencil_testFront = ComparisonFunctionFromString(compareFront_str);
        }

        if(auto xml_stencilback = xml_stencil->FirstChildElement("back")) {
            DataUtils::ValidateXmlElement(*xml_stencilback, "back", "", "fail,depthfail,pass,test");

            std::string failBack_str = "keep";
            failBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "fail", failBack_str);
            this->stencil_failBackOp = StencilOperationFromString(failBack_str);

            std::string depthfailBack_str = "keep";
            depthfailBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "depthfail", depthfailBack_str);
            this->stencil_failDepthBackOp = StencilOperationFromString(depthfailBack_str);

            std::string passBack_str = "keep";
            passBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "pass", passBack_str);
            this->stencil_passBackOp = StencilOperationFromString(passBack_str);

            std::string compareBack_str = "always";
            compareBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "test", compareBack_str);
            this->stencil_testBack = ComparisonFunctionFromString(compareBack_str);
        }
    }
}
