#pragma once

#include "Engine/RHI/RHITypes.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Math/IntVector2.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include <filesystem>
#include <memory>
#include <set>
#include <vector>

class RHIDeviceContext;
class RHIFactory;
class IntVector2;
class Window;
class RHIOutput;
class DepthStencilState;
class InputLayout;
class Vertex3D;
class ShaderProgram;

class RHIDevice {
public:
    RHIDevice() = default;
    ~RHIDevice() noexcept;

    std::pair<std::unique_ptr<RHIOutput>, std::unique_ptr<RHIDeviceContext>> CreateOutputAndContext(const IntVector2& clientSize, const IntVector2& clientPosition = IntVector2::ZERO, const RHIOutputMode& outputMode = RHIOutputMode::Windowed) noexcept;

    std::unique_ptr<VertexBuffer> CreateVertexBuffer(const VertexBuffer::buffer_t& vbo, const BufferUsage& usage, const BufferBindUsage& bindusage) const noexcept;
    std::unique_ptr<IndexBuffer> CreateIndexBuffer(const IndexBuffer::buffer_t& ibo, const BufferUsage& usage, const BufferBindUsage& bindusage) const noexcept;
    std::unique_ptr<InputLayout> CreateInputLayout() const noexcept;

    std::unique_ptr<StructuredBuffer> CreateStructuredBuffer(const StructuredBuffer::buffer_t& buffer, std::size_t element_size, std::size_t element_count, const BufferUsage& usage, const BufferBindUsage& bindUsage) const noexcept;
    std::unique_ptr<ConstantBuffer> CreateConstantBuffer(const ConstantBuffer::buffer_t& buffer, std::size_t buffer_size, const BufferUsage& usage, const BufferBindUsage& bindUsage) const noexcept;


    D3D_FEATURE_LEVEL GetFeatureLevel() const noexcept;
    ID3D11Device5* GetDxDevice() const noexcept;
    bool IsAllowTearingSupported() const noexcept;

    std::unique_ptr<ShaderProgram> CreateShaderProgramFromHlslString(const std::string& name, const std::string& hlslString, const std::string& entryPoint, std::unique_ptr<InputLayout> inputLayout, const PipelineStage& target) const noexcept;
    std::unique_ptr<ShaderProgram> CreateShaderProgramFromHlslFile(std::filesystem::path filepath, const std::string& entryPoint, const PipelineStage& target) const noexcept;

    ID3DBlob* CompileShader(const std::string& name, const void*  sourceCode, std::size_t sourceCodeSize, const std::string& entryPoint, const PipelineStage& target) const noexcept;
    std::vector<std::unique_ptr<ConstantBuffer>> CreateConstantBuffersFromByteCode(ID3DBlob* bytecode) const noexcept;

    mutable std::set<DisplayDesc, DisplayDescGTComparator> displayModes{};

private:
    std::pair<std::unique_ptr<RHIOutput>, std::unique_ptr<RHIDeviceContext>> CreateOutputAndContextFromWindow(std::unique_ptr<Window> window) noexcept;

    DeviceInfo CreateDeviceFromFirstAdapter(const std::vector<AdapterInfo>& adapters) noexcept;
    void OutputAdapterInfo(const std::vector<AdapterInfo>& adapters) const noexcept;
    void GetDisplayModes(const std::vector<AdapterInfo>& adapters) const noexcept;

    IDXGISwapChain4* CreateSwapChain(const Window& window, RHIFactory& factory) noexcept;

    std::vector<std::unique_ptr<ConstantBuffer>> CreateConstantBuffersUsingReflection(ID3D11ShaderReflection& cbufferReflection) const noexcept;
    std::unique_ptr<InputLayout> CreateInputLayoutFromByteCode(ID3DBlob* bytecode) const noexcept;

    std::vector<OutputInfo> GetOutputsFromAdapter(const AdapterInfo& a) const noexcept;
    void GetPrimaryDisplayModeDescriptions(const AdapterInfo& adapter, decltype(displayModes)& descriptions) const noexcept;
    void GetDisplayModeDescriptions(const AdapterInfo& adapter, const OutputInfo& output, decltype(displayModes)& descriptions) const noexcept;
    DisplayDesc GetDisplayModeMatchingDimensions(const std::vector<DisplayDesc>& descriptions, unsigned int w, unsigned int h) noexcept;

    ID3D11Device5* _dx_device = nullptr;
    D3D_FEATURE_LEVEL _dx_highestSupportedFeatureLevel{};
    bool _allow_tearing_supported = false;

    void SetupDebuggingInfo() noexcept;

};