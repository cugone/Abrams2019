#include "Engine/Renderer/Shader.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/InputLayout.hpp"
#include "Engine/Renderer/InputLayoutInstanced.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <system_error>

Shader::Shader(Renderer& renderer, ShaderProgram* shaderProgram /*= nullptr*/, DepthStencilState* depthStencil /*= nullptr*/, RasterState* rasterState /*= nullptr*/, BlendState* blendState /*= nullptr*/, Sampler* sampler /*= nullptr*/) noexcept
: _renderer(renderer)
, _shader_program(shaderProgram)
, _depth_stencil_state(depthStencil)
, _raster_state(rasterState)
, _blend_state(blendState)
, _sampler(sampler) {
    std::size_t count = renderer.GetShaderCount();
    _name += "_" + std::to_string(count);
}

Shader::Shader(Renderer& renderer, const XMLElement& element) noexcept
: _renderer(renderer) {
    std::size_t count = renderer.GetShaderCount();
    _name += "_" + std::to_string(count);

    LoadFromXml(element);
}

const std::string& Shader::GetName() const noexcept {
    return _name;
}

ShaderProgram* Shader::GetShaderProgram() const noexcept {
    return _shader_program;
}

RasterState* Shader::GetRasterState() const noexcept {
    return _raster_state;
}

DepthStencilState* Shader::GetDepthStencilState() const noexcept {
    return _depth_stencil_state.get();
}

BlendState* Shader::GetBlendState() const noexcept {
    return _blend_state.get();
}

Sampler* Shader::GetSampler() const noexcept {
    return _sampler;
}

std::vector<std::reference_wrapper<ConstantBuffer>> Shader::GetConstantBuffers() const noexcept {
    std::vector<std::reference_wrapper<ConstantBuffer>> cbufferRefs{};
    cbufferRefs.reserve(_cbuffers.size());
    for(auto& ptr : _cbuffers) {
        cbufferRefs.push_back(std::ref(*ptr));
    }
    cbufferRefs.shrink_to_fit();
    return cbufferRefs;
}

bool Shader::LoadFromXml(const XMLElement& element) noexcept {
    namespace FS = std::filesystem;
    DataUtils::ValidateXmlElement(element, "shader", "shaderprogram", "name", "depth,stencil,blends,raster,sampler,cbuffers");

    _name = DataUtils::ParseXmlAttribute(element, std::string("name"), _name);

    auto xml_SP = element.FirstChildElement("shaderprogram");
    DataUtils::ValidateXmlElement(*xml_SP, "shaderprogram", "", "src", "pipelinestages");

    FS::path p;
    {
        const std::string sp_src = DataUtils::ParseXmlAttribute(*xml_SP, "src", "");
        if(sp_src.empty()) {
            ERROR_AND_DIE("shaderprogram element has empty src attribute.");
        }
        p = FS::path(sp_src);
    }
    if(!StringUtils::StartsWith(p.string(), "__")) {
        std::error_code ec{};
        p = FS::canonical(p, ec);
        if(ec) {
            std::cout << ec.message();
            return false;
        }
    }
    p.make_preferred();
    if(nullptr == (_shader_program = _renderer.GetShaderProgram(p.string()))) {
        const bool is_hlsl = p.has_extension() && StringUtils::ToLowerCase(p.extension().string()) == ".hlsl";
        const bool is_cso = p.has_extension() && StringUtils::ToLowerCase(p.extension().string()) == ".cso";
        const bool is_valid_extension = is_hlsl || is_cso;
        GUARANTEE_OR_DIE(is_valid_extension, "ShaderProgram source path must be of type '.hlsl' or '.cso'");
        if(StringUtils::StartsWith(p.string(), "__")) {
            const auto ss = std::string{"Intrinsic ShaderProgram referenced in Shader file \""} + _name + "\" does not already exist.";
            ERROR_AND_DIE(ss.c_str());
        } else {
            if(is_hlsl) {
                const auto& children = DataUtils::GetChildElementNames(*xml_SP);
                if(std::find(std::begin(children), std::end(children), "pipelinestages") == std::end(children)) {
                    const auto ss = std::string{"User-defined ShaderProgram referenced in Shader file \""} + _name + "\" must declare pipelinestages in use.";
                    ERROR_AND_DIE(ss.c_str());
                }
            }
        }
        if(is_hlsl) {
            if(auto xml_pipelinestages = xml_SP->FirstChildElement("pipelinestages")) {
                DataUtils::ValidateXmlElement(*xml_pipelinestages, "pipelinestages", "", "", "vertex,hull,domain,geometry,pixel,compute", "");
                _renderer.CreateAndRegisterShaderProgramFromHlslFile(p.string(), ParseEntrypointList(*xml_pipelinestages), ParseTargets(*xml_pipelinestages));
                _shader_program = _renderer.GetShaderProgram(p.string());
            }
        } else if(is_cso) {
            ShaderProgramDesc desc{};
            desc.device = _renderer.GetDevice();
            desc.name = _name;
            DataUtils::ForEachChildElement(element, "shaderprogram", [this, &desc](const XMLElement& elem) {
                const auto sp_src = DataUtils::ParseXmlAttribute(elem, "src", "");
                auto p = FS::path(sp_src);
                std::error_code ec;
                p = FS::canonical(p, ec);
                GUARANTEE_OR_DIE(!ec, "Compiled shader source path is invalid:\n" + p.string());
                const auto has_filename = p.has_filename();
                GUARANTEE_OR_DIE(has_filename, "Compiled shader source path is not a file.");
                const auto filename = p.stem();
                const auto fn_str = filename.string();
                const auto is_vs = has_filename && StringUtils::EndsWith(fn_str, "_VS");
                const auto is_hs = has_filename && StringUtils::EndsWith(fn_str, "_HS");
                const auto is_ds = has_filename && StringUtils::EndsWith(fn_str, "_DS");
                const auto is_gs = has_filename && StringUtils::EndsWith(fn_str, "_GS");
                const auto is_ps = has_filename && StringUtils::EndsWith(fn_str, "_PS");
                const auto is_cs = has_filename && StringUtils::EndsWith(fn_str, "_CS");
                const auto has_valid_staged_filename = is_vs || is_hs || is_ds || is_gs || is_ps || is_cs;
                GUARANTEE_OR_DIE(has_valid_staged_filename, "Compiled shader source filename must end in '_VS' '_HS' '_DS' '_GS' '_PS' or '_CS'");
                auto buffer = FileUtils::ReadBinaryBufferFromFile(p);
                if(is_vs && buffer.has_value()) {
                    ID3DBlob* blob = nullptr;
                    auto hr = ::D3DCreateBlob(buffer->size(), &blob);
                    if(FAILED(hr)) {
                        DebuggerPrintf(StringUtils::FormatWindowsMessage(hr).c_str());
                        ERROR_AND_DIE("VS Blob creation failed.");
                    }
                    std::memcpy(blob->GetBufferPointer(), buffer->data(), blob->GetBufferSize());
                    buffer->clear();
                    buffer->shrink_to_fit();
                    desc.vs_bytecode = blob;
                    desc.device->GetDxDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &desc.vs);
                    desc.input_layout = desc.device->CreateInputLayoutFromByteCode(desc.vs_bytecode);
                } else if(is_hs && buffer.has_value()) {
                    ID3DBlob* blob = nullptr;
                    auto hr = ::D3DCreateBlob(buffer->size(), &blob);
                    if(FAILED(hr)) {
                        DebuggerPrintf(StringUtils::FormatWindowsMessage(hr).c_str());
                        ERROR_AND_DIE("HS Blob creation failed.");
                    }
                    std::memcpy(blob->GetBufferPointer(), buffer->data(), blob->GetBufferSize());
                    buffer->clear();
                    buffer->shrink_to_fit();
                    desc.hs_bytecode = blob;
                    desc.device->GetDxDevice()->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &desc.hs);
                } else if(is_ds && buffer.has_value()) {
                    ID3DBlob* blob = nullptr;
                    auto hr = ::D3DCreateBlob(buffer->size(), &blob);
                    if(FAILED(hr)) {
                        DebuggerPrintf(StringUtils::FormatWindowsMessage(hr).c_str());
                        ERROR_AND_DIE("DS Blob creation failed.");
                    }
                    std::memcpy(blob->GetBufferPointer(), buffer->data(), blob->GetBufferSize());
                    buffer->clear();
                    buffer->shrink_to_fit();
                    desc.ds_bytecode = blob;
                    desc.device->GetDxDevice()->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &desc.ds);
                } else if(is_gs && buffer.has_value()) {
                    ID3DBlob* blob = nullptr;
                    auto hr = ::D3DCreateBlob(buffer->size(), &blob);
                    if(FAILED(hr)) {
                        DebuggerPrintf(StringUtils::FormatWindowsMessage(hr).c_str());
                        ERROR_AND_DIE("GS Blob creation failed.");
                    }
                    std::memcpy(blob->GetBufferPointer(), buffer->data(), blob->GetBufferSize());
                    buffer->clear();
                    buffer->shrink_to_fit();
                    desc.gs_bytecode = blob;
                    desc.device->GetDxDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &desc.gs);
                } else if(is_ps && buffer.has_value()) {
                    ID3DBlob* blob = nullptr;
                    auto hr = ::D3DCreateBlob(buffer->size(), &blob);
                    if(FAILED(hr)) {
                        DebuggerPrintf(StringUtils::FormatWindowsMessage(hr).c_str());
                        ERROR_AND_DIE("PS Blob creation failed.");
                    }
                    std::memcpy(blob->GetBufferPointer(), buffer->data(), blob->GetBufferSize());
                    buffer->clear();
                    buffer->shrink_to_fit();
                    desc.ps_bytecode = blob;
                    desc.device->GetDxDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &desc.ps);
                } else if(is_cs && buffer.has_value()) {
                    ID3DBlob* blob = nullptr;
                    auto hr = ::D3DCreateBlob(buffer->size(), &blob);
                    if(FAILED(hr)) {
                        DebuggerPrintf(StringUtils::FormatWindowsMessage(hr).c_str());
                        ERROR_AND_DIE("CS Blob creation failed.");
                    }
                    std::memcpy(blob->GetBufferPointer(), buffer->data(), blob->GetBufferSize());
                    buffer->clear();
                    buffer->shrink_to_fit();
                    desc.cs_bytecode = blob;
                    desc.device->GetDxDevice()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &desc.cs);
                } else {
                    ERROR_AND_DIE("Could not determine shader type. Filename must end in _VS, _PS, _HS, _DS, _GS, or _CS.");
                }
            });
            auto sp = _renderer.CreateShaderProgramFromDesc(std::move(desc));
            auto* sp_ptr = sp.get();
            _renderer.RegisterShaderProgram(_name, std::move(sp));
            _shader_program = sp_ptr;
        }
    }
    _cbuffers = std::move(_renderer.CreateConstantBuffersFromShaderProgram(_shader_program));
    _depth_stencil_state = std::make_unique<DepthStencilState>(_renderer.GetDevice(), element);
    _blend_state = std::make_unique<BlendState>(_renderer.GetDevice(), element);

    _raster_state = _renderer.GetRasterState("__default");
    if(auto xml_raster = element.FirstChildElement("raster")) {
        std::string rs_src = DataUtils::ParseXmlAttribute(*xml_raster, "src", "");
        if(auto found_raster = _renderer.GetRasterState(rs_src)) {
            _raster_state = found_raster;
        } else {
            CreateAndRegisterNewRasterFromXml(element);
        }
    }

    _sampler = _renderer.GetSampler("__default");
    if(auto xml_sampler = element.FirstChildElement("sampler")) {
        std::string s_src = DataUtils::ParseXmlAttribute(*xml_sampler, "src", "");
        if(auto found_sampler = _renderer.GetSampler(s_src)) {
            _sampler = found_sampler;
        } else {
            CreateAndRegisterNewSamplerFromXml(element);
        }
    }
    return true;
}

PipelineStage Shader::ParseTargets(const XMLElement& element) noexcept {
    auto targets = PipelineStage::None;
    if(auto xml_vertex = element.FirstChildElement("vertex")) {
        targets |= PipelineStage::Vs;
    }
    if(auto xml_hull = element.FirstChildElement("hull")) {
        targets |= PipelineStage::Hs;
    }
    if(auto xml_domain = element.FirstChildElement("domain")) {
        targets |= PipelineStage::Ds;
    }
    if(auto xml_geometry = element.FirstChildElement("geometry")) {
        targets |= PipelineStage::Gs;
    }
    if(auto xml_pixel = element.FirstChildElement("pixel")) {
        targets |= PipelineStage::Ps;
    }
    if(auto xml_compute = element.FirstChildElement("compute")) {
        targets |= PipelineStage::Cs;
    }
    ValidatePipelineStages(targets);
    return targets;
}

std::string Shader::ParseEntrypointList(const XMLElement& element) noexcept {
    std::string entrypointList{};
    if(auto xml_vertex = element.FirstChildElement("vertex")) {
        auto entrypoint = DataUtils::ParseXmlAttribute(*xml_vertex, "entrypoint", "");
        entrypoint += ",";
        entrypointList += entrypoint;
    } else {
        entrypointList += ",";
    }
    if(auto xml_hull = element.FirstChildElement("hull")) {
        auto entrypoint = DataUtils::ParseXmlAttribute(*xml_hull, "entrypoint", "");
        entrypoint += ",";
        entrypointList += entrypoint;
    } else {
        entrypointList += ",";
    }
    if(auto xml_domain = element.FirstChildElement("domain")) {
        auto entrypoint = DataUtils::ParseXmlAttribute(*xml_domain, "entrypoint", "");
        entrypoint += ",";
        entrypointList += entrypoint;
    } else {
        entrypointList += ",";
    }
    if(auto xml_geometry = element.FirstChildElement("geometry")) {
        auto entrypoint = DataUtils::ParseXmlAttribute(*xml_geometry, "entrypoint", "");
        entrypoint += ",";
        entrypointList += entrypoint;
    } else {
        entrypointList += ",";
    }
    if(auto xml_pixel = element.FirstChildElement("pixel")) {
        auto entrypoint = DataUtils::ParseXmlAttribute(*xml_pixel, "entrypoint", "");
        entrypoint += ",";
        entrypointList += entrypoint;
    } else {
        entrypointList += ",";
    }
    if(auto xml_compute = element.FirstChildElement("compute")) {
        auto entrypoint = DataUtils::ParseXmlAttribute(*xml_compute, "entrypoint", "");
        entrypoint += ",";
        entrypointList += entrypoint;
    } else {
        entrypointList += ",";
    }
    return entrypointList;
}

void Shader::ValidatePipelineStages(const PipelineStage& targets) noexcept {
    bool result = false;
    if(targets == PipelineStage::None) {
        result = false;
    } else if(targets == PipelineStage::All) {
        result = true;
    } else {
        bool has_vs = (targets & PipelineStage::Vs) == PipelineStage::Vs;
        bool has_ps = (targets & PipelineStage::Ps) == PipelineStage::Ps;
        bool has_hs = (targets & PipelineStage::Hs) == PipelineStage::Hs;
        bool has_ds = (targets & PipelineStage::Ds) == PipelineStage::Ds;
        bool has_cs = (targets & PipelineStage::Cs) == PipelineStage::Cs;
        bool has_gs = (targets & PipelineStage::Gs) == PipelineStage::Gs;
        bool valid_vsps = !(has_vs ^ has_ps);
        bool valid_hsds = !(has_hs ^ has_ds);
        bool valid_cs = has_cs;
        bool valid_gs = has_gs;
        result = valid_cs || valid_gs || valid_vsps || valid_hsds;
    }
    if(!result) {
        const auto ss = std::string{"Error in shader file: \""} + _name + "\": Pipeline stages must include at least compute stage, geometry stage, or both vertex and pixel stages, or both hull and domain stages.";
        ERROR_AND_DIE(ss.c_str());
    }
}

void Shader::CreateAndRegisterNewSamplerFromXml(const XMLElement& element) noexcept {
    auto new_sampler = std::make_unique<Sampler>(_renderer.GetDevice(), element);
    std::string ns = _name + "_sampler";
    _sampler = new_sampler.get();
    _renderer.RegisterSampler(ns, std::move(new_sampler));
}

void Shader::CreateAndRegisterNewRasterFromXml(const XMLElement& element) noexcept {
    auto new_raster_state = std::make_unique<RasterState>(_renderer.GetDevice(), element);
    std::string nr = _name + "_raster";
    _raster_state = new_raster_state.get();
    _renderer.RegisterRasterState(nr, std::move(new_raster_state));
}
