#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Renderer/BlendState.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"

#include <memory>
#include <string>
#include <vector>

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

    const std::string& GetName() const noexcept;
    ShaderProgram* GetShaderProgram() const noexcept;
    RasterState* GetRasterState() const noexcept;
    DepthStencilState* GetDepthStencilState() const noexcept;
    BlendState* GetBlendState() const noexcept;
    Sampler* GetSampler() const noexcept;
    std::vector<std::reference_wrapper<ConstantBuffer>> GetConstantBuffers() const noexcept;

protected:
private:
    bool LoadFromXml(const XMLElement& element) noexcept;

    PipelineStage ParseTargets(const XMLElement& element) noexcept;
    std::string ParseEntrypointList(const XMLElement& element) noexcept;

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
};