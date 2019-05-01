#pragma once

#include "Engine/Core/DataUtils.hpp"

#include <string>

class ShaderProgram;
class RasterState;
class DepthStencilState;
class BlendState;
class Renderer;
class Sampler;
class ConstantBuffer;

enum class PipelineStage : uint8_t;

class Shader {
public:
    Shader(Renderer* renderer, ShaderProgram* shaderProgram = nullptr, DepthStencilState* depthStencil = nullptr, RasterState* rasterState = nullptr, BlendState* blendState = nullptr, Sampler* sampler = nullptr);
    Shader(Renderer* renderer, const XMLElement& element);
    ~Shader();

    const std::string& GetName() const;
    ShaderProgram* GetShaderProgram() const;
    RasterState* GetRasterState() const;
    DepthStencilState* GetDepthStencilState() const;
    BlendState* GetBlendState() const;
    Sampler* GetSampler() const;
    const std::vector<ConstantBuffer*>& GetConstantBuffers() const;

    void SetName(const std::string& name);
    void SetShaderProgram(ShaderProgram* sp);
    void SetRasterState(RasterState* rs);
    void SetDepthStencilState(DepthStencilState* ds);
    void SetBlendState(BlendState* bs);
    void SetSampler(Sampler* sampler);
    void SetConstantBuffers(const std::vector<ConstantBuffer*>& cbuffers);
protected:
private:
    bool LoadFromXml(Renderer* renderer, const XMLElement& element);

    PipelineStage ParseTargets(const XMLElement& element);
    std::string ParseEntrypointList(const XMLElement& element);

    void ValidatePipelineStages(const PipelineStage& targets);

    void CreateAndRegisterNewSamplerFromXml(const XMLElement& element);
    void CreateAndRegisterNewRasterFromXml(const XMLElement& element);

    std::string _name = "SHADER";
    Renderer* _renderer = nullptr;
    ShaderProgram* _shader_program = nullptr;
    DepthStencilState* _depth_stencil_state = nullptr;
    RasterState* _raster_state = nullptr;
    BlendState* _blend_state = nullptr;
    Sampler* _sampler = nullptr;
    std::vector<ConstantBuffer*> _cbuffers{};
    bool _raster_from_db = false;
    bool _sampler_from_db = false;
};