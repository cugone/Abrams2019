#pragma once

#include "Engine/Math/IntVector2.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIFactory.hpp"
#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/RHITypes.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DirectX/DX11.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include <filesystem>
#include <memory>
#include <set>
#include <vector>

#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable : 26812) // The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum'.
#endif

namespace a2de {

    class RHIDeviceContext;
    class RHIFactory;
    class IntVector2;
    class Window;
    class RHIOutput;
    class DepthStencilState;
    class InputLayout;
    class InputLayoutInstanced;
    struct Vertex3D;
    class ShaderProgram;
    class Renderer;
    struct WindowDesc;

    class RHIDevice {
    public:
        explicit RHIDevice(Renderer& parent_renderer) noexcept;
        ~RHIDevice() = default;

        [[nodiscard]] std::pair<std::unique_ptr<RHIOutput>, std::unique_ptr<RHIDeviceContext>> CreateOutputAndContext(const IntVector2& clientSize, const IntVector2& clientPosition = IntVector2::ZERO) noexcept;
        [[nodiscard]] std::pair<std::unique_ptr<RHIOutput>, std::unique_ptr<RHIDeviceContext>> CreateOutputAndContext(const WindowDesc& desc) noexcept;

        [[nodiscard]] std::unique_ptr<VertexBuffer> CreateVertexBuffer(const VertexBuffer::buffer_t& vbo, const BufferUsage& usage, const BufferBindUsage& bindusage) const noexcept;
        [[nodiscard]] std::unique_ptr<IndexBuffer> CreateIndexBuffer(const IndexBuffer::buffer_t& ibo, const BufferUsage& usage, const BufferBindUsage& bindusage) const noexcept;
        [[nodiscard]] std::unique_ptr<InputLayout> CreateInputLayout() const noexcept;

        [[nodiscard]] std::unique_ptr<StructuredBuffer> CreateStructuredBuffer(const StructuredBuffer::buffer_t& buffer, std::size_t element_size, std::size_t element_count, const BufferUsage& usage, const BufferBindUsage& bindUsage) const noexcept;
        [[nodiscard]] std::unique_ptr<ConstantBuffer> CreateConstantBuffer(const ConstantBuffer::buffer_t& buffer, std::size_t buffer_size, const BufferUsage& usage, const BufferBindUsage& bindUsage) const noexcept;

        [[nodiscard]] D3D_FEATURE_LEVEL GetFeatureLevel() const noexcept;
        [[nodiscard]] ID3D11Device5* GetDxDevice() const noexcept;
        [[nodiscard]] IDXGISwapChain4* GetDxSwapChain() const noexcept;
        [[nodiscard]] bool IsAllowTearingSupported() const noexcept;

        [[nodiscard]] std::unique_ptr<ShaderProgram> CreateShaderProgramFromHlslString(const std::string& name, const std::string& hlslString, const std::string& entryPoint, std::unique_ptr<InputLayout> inputLayout, const PipelineStage& target) const noexcept;
        [[nodiscard]] std::unique_ptr<ShaderProgram> CreateShaderProgramFromHlslFile(std::filesystem::path filepath, const std::string& entryPoint, const PipelineStage& target) const noexcept;
        [[nodiscard]] std::unique_ptr<ShaderProgram> CreateShaderProgramFromCsoBinaryBuffer(std::vector<uint8_t>& compiledShader, const std::string& name, const PipelineStage& target) const noexcept;
        [[nodiscard]] std::unique_ptr<ShaderProgram> CreateShaderProgramFromCsoFile(std::filesystem::path filepath, const PipelineStage& target) const noexcept;

        [[nodiscard]] std::unique_ptr<InputLayout> CreateInputLayoutFromByteCode(ID3DBlob* bytecode) const noexcept;

        [[nodiscard]] ID3DBlob* CompileShader(const std::string& name, const void* sourceCode, std::size_t sourceCodeSize, const std::string& entryPoint, const PipelineStage& target) const noexcept;
        [[nodiscard]] std::vector<std::unique_ptr<ConstantBuffer>> CreateConstantBuffersFromByteCode(ID3DBlob* bytecode) const noexcept;

        mutable std::set<DisplayDesc, DisplayDescGTComparator> displayModes{};

        void ResetSwapChainForHWnd() const noexcept;
        [[nodiscard]] Renderer& GetRenderer() const noexcept;

        void HandleDeviceLost() const noexcept;
    private:
        [[nodiscard]] std::pair<std::unique_ptr<RHIOutput>, std::unique_ptr<RHIDeviceContext>> CreateOutputAndContextFromWindow(std::unique_ptr<Window> window) noexcept;

        [[nodiscard]] DeviceInfo CreateDeviceFromFirstAdapter(const std::vector<AdapterInfo>& adapters) noexcept;
        void OutputAdapterInfo(const std::vector<AdapterInfo>& adapters) const noexcept;
        void GetDisplayModes(const std::vector<AdapterInfo>& adapters) const noexcept;

        [[nodiscard]] Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(const Window& window) noexcept;
        [[nodiscard]] Microsoft::WRL::ComPtr<IDXGISwapChain4> RecreateSwapChain(const Window& window) noexcept;

        [[nodiscard]] std::vector<std::unique_ptr<ConstantBuffer>> CreateConstantBuffersUsingReflection(ID3D11ShaderReflection& cbufferReflection) const noexcept;
        [[nodiscard]] std::unique_ptr<InputLayoutInstanced> CreateInputLayoutInstancedFromByteCode(ID3DBlob* vs_bytecode) const noexcept;

        [[nodiscard]] std::vector<OutputInfo> GetOutputsFromAdapter(const AdapterInfo& a) const noexcept;
        void GetPrimaryDisplayModeDescriptions(const AdapterInfo& adapter, decltype(displayModes)& descriptions) const noexcept;
        void GetDisplayModeDescriptions(const AdapterInfo& adapter, const OutputInfo& output, decltype(displayModes)& descriptions) const noexcept;
        [[nodiscard]] DisplayDesc GetDisplayModeMatchingDimensions(const std::vector<DisplayDesc>& descriptions, unsigned int w, unsigned int h) noexcept;

        Renderer& _parent_renderer;
        RHIFactory _rhi_factory{};
        D3D_FEATURE_LEVEL _dx_highestSupportedFeatureLevel{};
        Microsoft::WRL::ComPtr<IDXGISwapChain4> _dxgi_swapchain{};
        Microsoft::WRL::ComPtr<ID3D11Device5> _dx_device{};
        bool _allow_tearing_supported = false;

        void SetupDebuggingInfo([[maybe_unused]] bool breakOnWarningSeverityOrLower = true) noexcept;
    };

} // namespace a2de

#if defined(_MSC_VER)
#pragma warning (pop)
#endif

