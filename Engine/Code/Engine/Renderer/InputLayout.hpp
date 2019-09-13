#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHITypes.hpp"

#include <vector>

class RHIDevice;

class InputLayout {
public:
    explicit InputLayout(const RHIDevice* parent_device) noexcept;
    ~InputLayout() noexcept;

    void AddElement(std::size_t memberByteOffset, const ImageFormat& format, const char* semantic, unsigned int inputSlot = 0, bool isVertexData = true, unsigned int instanceDataStepRate = 0) noexcept;
    void AddElement(const D3D11_INPUT_ELEMENT_DESC& desc) noexcept;
    void CreateInputLayout(void* byte_code, std::size_t byte_code_length) noexcept;
    ID3D11InputLayout* GetDxInputLayout() const noexcept;
    void PopulateInputLayoutUsingReflection(ID3D11ShaderReflection& vertexReflection) noexcept;
protected:
private:
    D3D11_INPUT_ELEMENT_DESC CreateInputElementFromSignature(D3D11_SIGNATURE_PARAMETER_DESC& input_desc, unsigned int& last_input_slot) noexcept;

    std::vector<D3D11_INPUT_ELEMENT_DESC> _elements;
    ID3D11InputLayout* _dx_input_layout = nullptr;
    const RHIDevice* _parent_device = nullptr;
};