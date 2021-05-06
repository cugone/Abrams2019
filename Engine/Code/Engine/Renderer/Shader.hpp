#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Renderer/BlendState.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"

#include <memory>
#include <string>
#include <vector>

namespace a2de {

    class ShaderProgram;
    class RasterState;
    class Renderer;
    class Sampler;
    class ConstantBuffer;

    enum class PipelineStage : uint8_t;

    class Shader {
    public:
        explicit Shader(Renderer& renderer, ShaderProgram* shaderProgram = nullptr, DepthStencilState* depthStencil = nullptr, RasterState* rasterState = nullptr, BlendState* blendState = nullptr, Sampler* sampler = nullptr) noexcept;
        Shader(Renderer& renderer, const XMLElement& element) noexcept;
        ~Shader() = default;

        [[nodiscard]] const std::string& GetName() const noexcept;
        [[nodiscard]] ShaderProgram* GetShaderProgram() const noexcept;
        [[nodiscard]] RasterState* GetRasterState() const noexcept;
        [[nodiscard]] DepthStencilState* GetDepthStencilState() const noexcept;
        [[nodiscard]] BlendState* GetBlendState() const noexcept;
        [[nodiscard]] Sampler* GetSampler() const noexcept;
        [[nodiscard]] std::vector<std::reference_wrapper<ConstantBuffer>> GetConstantBuffers() const noexcept;
        [[nodiscard]] std::vector<std::reference_wrapper<ConstantBuffer>> GetComputeConstantBuffers() const noexcept;

    protected:
    private:
        bool LoadFromXml(const XMLElement& element) noexcept;

        [[nodiscard]] PipelineStage ParseTargets(const XMLElement& element) noexcept;
        [[nodiscard]] std::string ParseEntrypointList(const XMLElement& element) noexcept;

        void ValidatePipelineStages(const PipelineStage& targets) noexcept;

        void CreateAndRegisterNewSamplerFromXml(const XMLElement& element) noexcept;
        void CreateAndRegisterNewRasterFromXml(const XMLElement& element) noexcept;

        std::string _name = "SHADER";
        Renderer& _renderer;
        ShaderProgram* _shader_program = nullptr;
        std::unique_ptr<DepthStencilState> _depth_stencil_state;
        RasterState* _raster_state = nullptr;
        std::unique_ptr<BlendState> _blend_state;
        Sampler* _sampler = nullptr;
        std::vector<std::unique_ptr<ConstantBuffer>> _cbuffers;
        std::vector<std::unique_ptr<ConstantBuffer>> _ccbuffers;
    };

} // namespace a2de
