#include "Engine/Renderer/ShaderProgram.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"
#include "Engine/Renderer/InputLayout.hpp"

#include "Engine/RHI/RHIDevice.hpp"

ShaderProgram::ShaderProgram(ShaderProgramDesc&& desc)
    : _desc(std::move(desc))
{
    /* DO NOTHING */
}

ShaderProgramDesc&& ShaderProgram::GetDescription() {
    return std::move(_desc);
}

void ShaderProgram::SetDescription(ShaderProgramDesc&& description) {
    _desc = std::move(description);
}

const std::string& ShaderProgram::GetName() const {
    return _desc.name;
}

const RHIDevice* ShaderProgram::GetParentDevice() const {
    return _desc.device;
}

ID3DBlob* ShaderProgram::GetVSByteCode() const {
    return _desc.vs_bytecode;
}

ID3DBlob* ShaderProgram::GetHSByteCode() const {
    return _desc.hs_bytecode;
}

ID3DBlob* ShaderProgram::GetDSByteCode() const {
    return _desc.ds_bytecode;
}

ID3DBlob* ShaderProgram::GetGSByteCode() const {
    return _desc.gs_bytecode;
}

ID3DBlob* ShaderProgram::GetPSByteCode() const {
    return _desc.ps_bytecode;
}

ID3DBlob* ShaderProgram::GetCSByteCode() const {
    return _desc.cs_bytecode;
}

InputLayout* ShaderProgram::GetInputLayout() const {
    return _desc.input_layout;
}

ID3D11VertexShader* ShaderProgram::GetVS() const {
    return _desc.vs;
}

ID3D11HullShader* ShaderProgram::GetHS() const {
    return _desc.hs;
}

ID3D11DomainShader* ShaderProgram::GetDS() const {
    return _desc.ds;
}

ID3D11GeometryShader* ShaderProgram::GetGS() const {
    return _desc.gs;
}

ID3D11PixelShader* ShaderProgram::GetPS() const {
    return _desc.ps;
}

ID3D11ComputeShader* ShaderProgram::GetCS() const {
    return _desc.cs;
}

ShaderProgramDesc::ShaderProgramDesc(ShaderProgramDesc&& other) {
    name = std::move(other.name);

    device = other.device;
    other.device = nullptr;

    input_layout = other.input_layout;
    other.input_layout = nullptr;

    vs = other.vs;
    vs_bytecode = other.vs_bytecode;
    other.vs = nullptr;
    other.vs_bytecode = nullptr;

    ps = other.ps;
    ps_bytecode = other.ps_bytecode;
    other.ps = nullptr;
    other.ps_bytecode = nullptr;

    hs = other.hs;
    hs_bytecode = other.hs_bytecode;
    other.hs = nullptr;
    other.hs_bytecode = nullptr;

    ds = other.ds;
    ds_bytecode = other.ds_bytecode;
    other.ds = nullptr;
    other.ds_bytecode = nullptr;

    gs = other.gs;
    gs_bytecode = other.gs_bytecode;
    other.gs = nullptr;
    other.gs_bytecode = nullptr;

    cs = other.cs;
    cs_bytecode = other.cs_bytecode;
    other.cs = nullptr;
    other.cs_bytecode = nullptr;

}

ShaderProgramDesc& ShaderProgramDesc::operator=(ShaderProgramDesc&& other) {
    name = std::move(other.name);

    device = other.device;
    other.device = nullptr;

    input_layout = other.input_layout;
    other.input_layout = nullptr;

    vs = other.vs;
    vs_bytecode = other.vs_bytecode;
    other.vs = nullptr;
    other.vs_bytecode = nullptr;

    ps = other.ps;
    ps_bytecode = other.ps_bytecode;
    other.ps = nullptr;
    other.ps_bytecode = nullptr;

    hs = other.hs;
    hs_bytecode = other.hs_bytecode;
    other.hs = nullptr;
    other.hs_bytecode = nullptr;

    ds = other.ds;
    ds_bytecode = other.ds_bytecode;
    other.ds = nullptr;
    other.ds_bytecode = nullptr;

    gs = other.gs;
    gs_bytecode = other.gs_bytecode;
    other.gs = nullptr;
    other.gs_bytecode = nullptr;

    cs = other.cs;
    cs_bytecode = other.cs_bytecode;
    other.cs = nullptr;
    other.cs_bytecode = nullptr;

    return *this;
}

ShaderProgramDesc::~ShaderProgramDesc() {
    if(vs_bytecode) {
        vs_bytecode->Release();
        vs_bytecode = nullptr;
    }
    if(hs_bytecode) {
        hs_bytecode->Release();
        hs_bytecode = nullptr;
    }
    if(ds_bytecode) {
        ds_bytecode->Release();
        ds_bytecode = nullptr;
    }
    if(gs_bytecode) {
        gs_bytecode->Release();
        gs_bytecode = nullptr;
    }
    if(ps_bytecode) {
        ps_bytecode->Release();
        ps_bytecode = nullptr;
    }
    if(cs_bytecode) {
        cs_bytecode->Release();
        cs_bytecode = nullptr;
    }

    delete input_layout;
    input_layout = nullptr;

    if(vs) {
        vs->Release();
        vs = nullptr;
    }
    if(hs) {
        hs->Release();
        hs = nullptr;
    }
    if(ds) {
        ds->Release();
        ds = nullptr;
    }
    if(gs) {
        gs->Release();
        gs = nullptr;
    }
    if(ps) {
        ps->Release();
        ps = nullptr;
    }
    if(cs) {
        cs->Release();
        cs = nullptr;
    }
}
