#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"

class InputLayout;
class RHIDevice;

struct ShaderProgramDesc {
    std::string name{ "UNNAMED SHADER PROGRAM" };
    const RHIDevice* device   = nullptr;
    ID3D11VertexShader* vs    = nullptr;
    ID3D11PixelShader* ps     = nullptr;
    ID3DBlob* vs_bytecode     = nullptr;
    ID3DBlob* ps_bytecode     = nullptr;
    InputLayout* input_layout = nullptr;
    ID3D11HullShader* hs      = nullptr;
    ID3DBlob* hs_bytecode     = nullptr;
    ID3D11DomainShader* ds    = nullptr;
    ID3DBlob* ds_bytecode     = nullptr;
    ID3D11GeometryShader* gs  = nullptr;
    ID3DBlob* gs_bytecode     = nullptr;
    ID3D11ComputeShader* cs   = nullptr;
    ID3DBlob* cs_bytecode     = nullptr;
    ShaderProgramDesc() = default;
    ShaderProgramDesc(ShaderProgramDesc&& other);
    ShaderProgramDesc& operator=(ShaderProgramDesc&& other);
    ShaderProgramDesc(const ShaderProgramDesc& other) = delete;
    ShaderProgramDesc& operator=(const ShaderProgramDesc& other) = delete;
    ~ShaderProgramDesc();
};

class ShaderProgram {
public:
    explicit ShaderProgram(ShaderProgramDesc&& desc);
    ShaderProgram(ShaderProgram&& other) = default;
    ShaderProgram& operator=(ShaderProgram&& other) = default;
    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram& operator=(const ShaderProgram& other) = delete;
    ~ShaderProgram() = default;

    ShaderProgramDesc&& GetDescription();
    void SetDescription(ShaderProgramDesc&& description);

    const std::string& GetName() const;
    const RHIDevice* GetParentDevice() const;
    ID3DBlob* GetVSByteCode() const;
    ID3DBlob* GetHSByteCode() const;
    ID3DBlob* GetDSByteCode() const;
    ID3DBlob* GetGSByteCode() const;
    ID3DBlob* GetPSByteCode() const;
    ID3DBlob* GetCSByteCode() const;
    InputLayout* GetInputLayout() const;
    ID3D11VertexShader* GetVS() const;
    ID3D11HullShader* GetHS() const;
    ID3D11DomainShader* GetDS() const;
    ID3D11GeometryShader* GetGS() const;
    ID3D11PixelShader* GetPS() const;
    ID3D11ComputeShader* GetCS() const;

protected:
private:
    ShaderProgramDesc _desc{};
};