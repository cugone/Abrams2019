#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHITypes.hpp"

#include <vector>

class RHIDevice;

class InputLayout {
public:
    explicit InputLayout(const RHIDevice* parent_device);
    ~InputLayout();

    void AddElement(std::size_t memberByteOffset, const ImageFormat& format, const char* semantic, unsigned int inputSlot = 0, bool isVertexData = true, unsigned int instanceDataStepRate = 0);
    void AddElement(const D3D11_INPUT_ELEMENT_DESC& desc);
    void CreateInputLayout(void* byte_code, std::size_t byte_code_length);
    ID3D11InputLayout* GetDxInputLayout() const;
    void PopulateInputLayoutUsingReflection(ID3D11ShaderReflection& vertexReflection);
protected:
private:
    D3D11_INPUT_ELEMENT_DESC CreateInputElementFromSignature(D3D11_SIGNATURE_PARAMETER_DESC& input_desc, unsigned int& last_input_slot);

    std::vector<D3D11_INPUT_ELEMENT_DESC> _elements;
    ID3D11InputLayout* _dx_input_layout = nullptr;
    const RHIDevice* _parent_device = nullptr;
};