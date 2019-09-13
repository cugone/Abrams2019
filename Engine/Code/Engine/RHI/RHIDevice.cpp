#include "Engine/RHI/RHIDevice.hpp"

#include "Engine/Core/BuildConfig.hpp"

#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIFactory.hpp"

#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/InputLayout.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Window.hpp"

#include <array>
#include <sstream>

RHIDevice::~RHIDevice() {
    if(_dx_device) {
        _dx_device->Release();
        _dx_device = nullptr;
    }
}

std::pair<std::unique_ptr<RHIOutput>, std::unique_ptr<RHIDeviceContext>> RHIDevice::CreateOutputAndContext(const IntVector2& clientSize, const IntVector2& clientPosition /*= IntVector2::ZERO*/, const RHIOutputMode& outputMode /*= RHIOutputMode::WINDOWED*/) noexcept {
    Window* window = new Window;
    window->SetDimensionsAndPosition(clientPosition, clientSize);
    window->SetDisplayMode(outputMode);
    return CreateOutputAndContextFromWindow(window);
}

D3D_FEATURE_LEVEL RHIDevice::GetFeatureLevel() const noexcept {
    return _dx_highestSupportedFeatureLevel;
}

ID3D11Device5* RHIDevice::GetDxDevice() const noexcept {
    return _dx_device;
}

bool RHIDevice::IsAllowTearingSupported() const noexcept {
    return _allow_tearing_supported;
}

std::unique_ptr<VertexBuffer> RHIDevice::CreateVertexBuffer(const VertexBuffer::buffer_t& vbo, const BufferUsage& usage, const BufferBindUsage& bindusage) const noexcept {
    return std::move(std::make_unique<VertexBuffer>(this, vbo, usage, bindusage));
}

std::unique_ptr<IndexBuffer> RHIDevice::CreateIndexBuffer(const IndexBuffer::buffer_t& ibo, const BufferUsage& usage, const BufferBindUsage& bindusage) const noexcept {
    return std::move(std::make_unique<IndexBuffer>(this, ibo, usage, bindusage));
}

std::unique_ptr<InputLayout> RHIDevice::CreateInputLayout() const noexcept {
    return std::move(std::make_unique<InputLayout>(this));
}

std::unique_ptr<StructuredBuffer> RHIDevice::CreateStructuredBuffer(const StructuredBuffer::buffer_t& buffer, std::size_t element_size, std::size_t element_count, const BufferUsage& usage, const BufferBindUsage& bindUsage) const noexcept {
    return std::move(std::make_unique<StructuredBuffer>(this, buffer, element_size, element_count, usage, bindUsage));
}

std::unique_ptr<ConstantBuffer> RHIDevice::CreateConstantBuffer(const ConstantBuffer::buffer_t& buffer, std::size_t buffer_size, const BufferUsage& usage, const BufferBindUsage& bindUsage) const noexcept {
    return std::move(std::make_unique<ConstantBuffer>(this, buffer, buffer_size, usage, bindUsage));
}

std::pair<std::unique_ptr<RHIOutput>, std::unique_ptr<RHIDeviceContext>> RHIDevice::CreateOutputAndContextFromWindow(Window*& window) noexcept {

    if(window == nullptr) {
        ERROR_AND_DIE("RHIDevice: Invalid Window!");
    }

    window->Open();
    RHIFactory factory{};
    factory.RestrictAltEnterToggle(*window);

    std::vector<AdapterInfo> adapters = factory.GetAdaptersByHighPerformancePreference();
    if(adapters.empty()) {
        delete window;
        window = nullptr;
        ERROR_AND_DIE("RHIDevice: Graphics card not found.")
    }
    OutputAdapterInfo(adapters);

    auto device_info = CreateDeviceFromFirstAdapter(adapters);
    _dx_device = device_info.dx_device;
    _dx_highestSupportedFeatureLevel = device_info.highest_supported_feature_level;

    auto dxgi_swap_chain = CreateSwapChain(*window, factory);
    _allow_tearing_supported = factory.QueryForAllowTearingSupport();

    GetDisplayModes(adapters);
    for(auto& info : adapters) {
        info.Release();
    }
    SetupDebuggingInfo();

    return std::make_pair(
        std::move(std::make_unique<RHIOutput>(this, window, dxgi_swap_chain)),
        std::move(std::make_unique<RHIDeviceContext>(this, device_info.dx_context)));
}

DeviceInfo RHIDevice::CreateDeviceFromFirstAdapter(const std::vector<AdapterInfo>& adapters) noexcept {
    DeviceInfo info{};

    unsigned int device_flags = 0U;
#ifdef RENDER_DEBUG
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    std::array feature_levels{
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1,
    };
    auto first_adapter_info = std::begin(adapters);
    std::ostringstream ss;
    ss << "Selected Adapter: " << AdapterInfoToGraphicsCardDesc(*first_adapter_info).Description << std::endl;
    DebuggerPrintf(ss.str().c_str());
    auto first_adapter = first_adapter_info->adapter;
    bool has_adapter = first_adapter != nullptr;
    ID3D11Device* temp_device{};
    auto hr_device = ::D3D11CreateDevice(has_adapter ? first_adapter : nullptr
        , has_adapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE
        , nullptr
        , device_flags
        , feature_levels.data()
        , static_cast<unsigned int>(feature_levels.size())
        , D3D11_SDK_VERSION
        , &temp_device
        , &info.highest_supported_feature_level
        , &info.dx_context);
    GUARANTEE_OR_DIE(SUCCEEDED(hr_device), "Failed to create device.");
    auto hr_dxdevice5i = temp_device->QueryInterface(__uuidof(ID3D11Device5), (void**)&info.dx_device);
    GUARANTEE_OR_DIE(SUCCEEDED(hr_dxdevice5i), "Failed to upgrade to ID3D11Device5.");
    temp_device->Release();
    temp_device = nullptr;
    return info;
}

void RHIDevice::OutputAdapterInfo(const std::vector<AdapterInfo>& adapters) const noexcept {
    std::ostringstream ss;
    ss << "ADAPTERS\n";
    for(const auto& adapter : adapters) {
        ss << std::right << std::setw(60) << std::setfill('-') << '\n' << std::setfill(' ');
        ss << AdapterInfoToGraphicsCardDesc(adapter) << '\n';
    }
    ss << std::right << std::setw(60) << std::setfill('-') << '\n';
    ss << std::flush;
    DebuggerPrintf(ss.str().c_str());
}

void RHIDevice::GetDisplayModes(const std::vector<AdapterInfo>& adapters) const noexcept {
    for(auto& a : adapters) {
        auto&& outputs = GetOutputsFromAdapter(a);
        for(const auto& o : outputs) {
            GetDisplayModeDescriptions(a, o, displayModes);
        }
        for(auto& o : outputs) {
            o.output->Release();
            o.output = nullptr;
        }
    }
}

IDXGISwapChain4* RHIDevice::CreateSwapChain(const Window& window, RHIFactory& factory) noexcept {
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc{};
    auto window_dims = window.GetDimensions();
    auto width = static_cast<unsigned int>(window_dims.x);
    auto height = static_cast<unsigned int>(window_dims.y);
    swap_chain_desc.Width = width;
    swap_chain_desc.Height = height;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.Stereo = FALSE;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    IDXGISwapChain4* dxgi_swap_chain = factory.CreateSwapChainForHwnd(this, window, swap_chain_desc);
    return dxgi_swap_chain;
}

std::vector<OutputInfo> RHIDevice::GetOutputsFromAdapter(const AdapterInfo& a) const noexcept {
    if(!a.adapter) {
        return{};
    }
    std::vector<OutputInfo> outputs{};
    unsigned int i = 0u;
    IDXGIOutput6* cur_output = nullptr;
    while(a.adapter->EnumOutputs(i++, reinterpret_cast<IDXGIOutput**>(&cur_output)) != DXGI_ERROR_NOT_FOUND) {
        OutputInfo cur_info{};
        cur_info.output = cur_output;
        cur_output->GetDesc1(&cur_info.desc);
        outputs.push_back(cur_info);
    }
    return std::move(outputs);
}

void RHIDevice::GetPrimaryDisplayModeDescriptions(const AdapterInfo& adapter, decltype(displayModes)& descriptions) const noexcept {
    auto&& outputs = GetOutputsFromAdapter(adapter);
    if(outputs.empty()) {
        return;
    }
    GetDisplayModeDescriptions(adapter, outputs.front(), descriptions);
    outputs.clear();
    outputs.shrink_to_fit();
}

void RHIDevice::GetDisplayModeDescriptions(const AdapterInfo& adapter, const OutputInfo& output, decltype(displayModes)& descriptions) const noexcept {
    if(!adapter.adapter) {
        return;
    }
    if(!output.output) {
        return;
    }
    unsigned int display_count = 0u;
    unsigned int display_mode_flags = DXGI_ENUM_MODES_SCALING | DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_STEREO | DXGI_ENUM_MODES_DISABLED_STEREO;

    //Call with nullptr to get display count;
    output.output->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, display_mode_flags, &display_count, nullptr);
    if(display_count == 0) {
        return;
    }

    //Call again to fill array.
    std::vector<DXGI_MODE_DESC1> dxgi_descriptions(static_cast<std::size_t>(display_count), DXGI_MODE_DESC1{});
    output.output->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, display_mode_flags, &display_count, dxgi_descriptions.data());

    for(const auto& dxgi_desc : dxgi_descriptions) {
        DisplayDesc display{};
        display.width = dxgi_desc.Width;
        display.height = dxgi_desc.Height;
        display.refreshRateHz = dxgi_desc.RefreshRate.Numerator / dxgi_desc.RefreshRate.Denominator;
        descriptions.insert(display);
    }
}

DisplayDesc RHIDevice::GetDisplayModeMatchingDimensions(const std::vector<DisplayDesc>& descriptions, unsigned int w, unsigned int h) noexcept {
    for(const auto& desc : descriptions) {
        if(desc.width == w && desc.height == h) {
            return desc;
        }
    }
    return{};
}

void RHIDevice::SetupDebuggingInfo() noexcept {
#ifdef RENDER_DEBUG
    ID3D11Debug* _dx_debug = nullptr;
    if(SUCCEEDED(_dx_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&_dx_debug))) {
        ID3D11InfoQueue* _dx_infoqueue = nullptr;
        if(SUCCEEDED(_dx_debug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&_dx_infoqueue))) {
            _dx_infoqueue->SetMuteDebugOutput(false);
            _dx_infoqueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            _dx_infoqueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_ERROR, true);
            _dx_infoqueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_WARNING, true);
            _dx_infoqueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_INFO, true);
            _dx_infoqueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_MESSAGE, true);
            std::vector<D3D11_MESSAGE_ID> hidden = {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
            };
            D3D11_INFO_QUEUE_FILTER filter{};
            filter.DenyList.NumIDs = static_cast<unsigned int>(hidden.size());
            filter.DenyList.pIDList = hidden.data();
            _dx_infoqueue->AddStorageFilterEntries(&filter);
            _dx_infoqueue->Release();
            _dx_infoqueue = nullptr;
        }
        _dx_debug->Release();
        _dx_debug = nullptr;
    }
#endif
}

std::vector<std::unique_ptr<ConstantBuffer>> RHIDevice::CreateConstantBuffersFromByteCode(ID3DBlob* bytecode) const noexcept {
    if(!bytecode) {
        return {};
    }
    ID3D11ShaderReflection* cbufferReflection = nullptr;
    if(FAILED(::D3DReflect(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&cbufferReflection))) {
        return {};
    }
    auto cbuffers = std::move(CreateConstantBuffersUsingReflection(*cbufferReflection));
    cbufferReflection->Release();
    cbufferReflection = nullptr;
    return std::move(cbuffers);
}

std::vector<std::unique_ptr<ConstantBuffer>> RHIDevice::CreateConstantBuffersUsingReflection(ID3D11ShaderReflection& cbufferReflection) const noexcept {
    D3D11_SHADER_DESC shader_desc{};
    if(FAILED(cbufferReflection.GetDesc(&shader_desc))) {
        return {};
    }
    if(!shader_desc.ConstantBuffers) {
        return{};
    }

    std::vector<std::unique_ptr<ConstantBuffer>> result{};
    result.reserve(shader_desc.ConstantBuffers);
    for(auto resource_idx = 0u; resource_idx < shader_desc.BoundResources; ++resource_idx) {
        D3D11_SHADER_INPUT_BIND_DESC input_desc{};
        if(FAILED(cbufferReflection.GetResourceBindingDesc(resource_idx, &input_desc))) {
            continue;
        }
        if(input_desc.Type != D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER) {
            continue;
        }
        if(input_desc.BindPoint < Renderer::CONSTANT_BUFFER_START_INDEX) {
            continue;
        }
        for(auto cbuffer_idx = 0u; cbuffer_idx < shader_desc.ConstantBuffers; ++cbuffer_idx) {
            ID3D11ShaderReflectionConstantBuffer* reflected_cbuffer = nullptr;
            if(nullptr == (reflected_cbuffer = cbufferReflection.GetConstantBufferByIndex(cbuffer_idx))) {
                continue;
            }
            D3D11_SHADER_BUFFER_DESC buffer_desc{};
            if(FAILED(reflected_cbuffer->GetDesc(&buffer_desc))) {
                continue;
            }
            if(buffer_desc.Type != D3D_CBUFFER_TYPE::D3D11_CT_CBUFFER) {
                continue;
            }
            std::string buffer_name{ buffer_desc.Name ? buffer_desc.Name : "" };
            std::string input_name{ input_desc.Name ? input_desc.Name : "" };
            if(buffer_name != input_name) {
                continue;
            }
            std::size_t cbuffer_size = 0u;
            std::vector<std::size_t> var_offsets{};
            for(auto variable_idx = 0u; variable_idx < buffer_desc.Variables; ++variable_idx) {
                ID3D11ShaderReflectionVariable* reflected_variable = nullptr;
                if(nullptr == (reflected_variable = reflected_cbuffer->GetVariableByIndex(variable_idx))) {
                    continue;
                }
                D3D11_SHADER_VARIABLE_DESC variable_desc{};
                if(FAILED(reflected_variable->GetDesc(&variable_desc))) {
                    continue;
                }
                std::size_t variable_size = variable_desc.Size;
                std::size_t offset = variable_desc.StartOffset;
                if(auto shader_reflection_type = reflected_variable->GetType()) {
                    D3D11_SHADER_TYPE_DESC type_desc{};
                    if(FAILED(shader_reflection_type->GetDesc(&type_desc))) {
                        continue;
                    }
                    cbuffer_size += variable_size;
                    var_offsets.push_back(offset);
                }
            }
            std::vector<std::byte> cbuffer_memory{};
            cbuffer_memory.resize(cbuffer_size);
            result.push_back(std::move(CreateConstantBuffer(cbuffer_memory.data(), cbuffer_memory.size(), BufferUsage::Dynamic, BufferBindUsage::Constant_Buffer)));
        }
    }
    return std::move(result);
}

std::unique_ptr<InputLayout> RHIDevice::CreateInputLayoutFromByteCode(ID3DBlob* bytecode) const noexcept {
    ID3D11ShaderReflection* vertexReflection = nullptr;
    if(FAILED(::D3DReflect(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vertexReflection))) {
        return nullptr;
    }
    auto il = std::make_unique<InputLayout>(this);
    il->PopulateInputLayoutUsingReflection(*vertexReflection);
    il->CreateInputLayout(bytecode->GetBufferPointer(), bytecode->GetBufferSize());
    return std::move(il);
}

std::unique_ptr<ShaderProgram> RHIDevice::CreateShaderProgramFromHlslString(const std::string& name, const std::string& hlslString, const std::string& entryPointList, std::unique_ptr<InputLayout> inputLayout, const PipelineStage& target) const noexcept {
    bool uses_vs_stage = static_cast<unsigned char>(target & PipelineStage::Vs) != 0;
    bool uses_hs_stage = static_cast<unsigned char>(target & PipelineStage::Hs) != 0;
    bool uses_ds_stage = static_cast<unsigned char>(target & PipelineStage::Ds) != 0;
    bool uses_gs_stage = static_cast<unsigned char>(target & PipelineStage::Gs) != 0;
    bool uses_ps_stage = static_cast<unsigned char>(target & PipelineStage::Ps) != 0;
    bool uses_cs_stage = static_cast<unsigned char>(target & PipelineStage::Cs) != 0;

    auto entrypoints = StringUtils::Split(entryPointList, ',', false);

    enum class EntrypointIndexes : unsigned char {
        Vs,
        Hs,
        Ds,
        Gs,
        Ps,
        Cs,
    };

    ShaderProgramDesc desc{};
    desc.device = this;
    desc.name = name;
    if(uses_vs_stage) {
        std::string stage = ":VS";
        ID3DBlob* vs_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Vs)], PipelineStage::Vs);
        if(!vs_bytecode) {
            return nullptr;
        }
        ID3D11VertexShader* vs = nullptr;
        _dx_device->CreateVertexShader(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize(), nullptr, &vs);
        if(inputLayout) {
            inputLayout->CreateInputLayout(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize());
        } else {
            inputLayout = CreateInputLayoutFromByteCode(vs_bytecode);
        }
        desc.vs = vs;
        desc.vs_bytecode = vs_bytecode;
        desc.input_layout = std::move(inputLayout);
    }

    if(uses_ps_stage) {
        std::string stage = ":PS";
        ID3DBlob* ps_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Ps)], PipelineStage::Ps);
        if(!ps_bytecode) {
            return nullptr;
        }
        ID3D11PixelShader* ps = nullptr;
        _dx_device->CreatePixelShader(ps_bytecode->GetBufferPointer(), ps_bytecode->GetBufferSize(), nullptr, &ps);
        desc.ps = ps;
        desc.ps_bytecode = ps_bytecode;
    }

    if(uses_hs_stage) {
        std::string stage = ":HS";
        ID3DBlob* hs_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Hs)], PipelineStage::Hs);
        if(!hs_bytecode) {
            return nullptr;
        }
        ID3D11HullShader* hs = nullptr;
        _dx_device->CreateHullShader(hs_bytecode->GetBufferPointer(), hs_bytecode->GetBufferSize(), nullptr, &hs);
        desc.hs = hs;
        desc.hs_bytecode = hs_bytecode;
    }

    if(uses_ds_stage) {
        std::string stage = ":DS";
        ID3DBlob* ds_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Ds)], PipelineStage::Ds);
        if(!ds_bytecode) {
            return nullptr;
        }
        ID3D11DomainShader* ds = nullptr;
        _dx_device->CreateDomainShader(ds_bytecode->GetBufferPointer(), ds_bytecode->GetBufferSize(), nullptr, &ds);
        desc.ds = ds;
        desc.ds_bytecode = ds_bytecode;
    }

    if(uses_gs_stage) {
        std::string stage = ":GS";
        ID3DBlob* gs_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Gs)], PipelineStage::Gs);
        if(!gs_bytecode) {
            return nullptr;
        }
        ID3D11GeometryShader* gs = nullptr;
        _dx_device->CreateGeometryShader(gs_bytecode->GetBufferPointer(), gs_bytecode->GetBufferSize(), nullptr, &gs);
        desc.gs = gs;
        desc.gs_bytecode = gs_bytecode;
    }

    if(uses_cs_stage) {
        std::string stage = ":CS";
        ID3DBlob* cs_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Cs)], PipelineStage::Cs);
        if(!cs_bytecode) {
            return nullptr;
        }
        ID3D11ComputeShader* cs = nullptr;
        _dx_device->CreateComputeShader(cs_bytecode->GetBufferPointer(), cs_bytecode->GetBufferSize(), nullptr, &cs);
        desc.cs = cs;
        desc.cs_bytecode = cs_bytecode;
    }
    return std::move(std::make_unique<ShaderProgram>(std::move(desc)));
}

std::unique_ptr<ShaderProgram> RHIDevice::CreateShaderProgramFromHlslFile(std::filesystem::path filepath, const std::string& entryPoint, const PipelineStage& target) const noexcept {
    bool retry_requested = false;
    do {
        std::string source{};
        if(FileUtils::ReadBufferFromFile(source, filepath)) {
            auto sp = CreateShaderProgramFromHlslString(filepath.string(), source, entryPoint, nullptr, target);
            if(sp) {
                return sp;
            }
            std::ostringstream ss;
            ss << "Shader program " << filepath << " failed to compile.\nSee Output window for errors.\nPress Retry to recompile.";
            retry_requested = IDRETRY == ::MessageBoxA(nullptr, ss.str().c_str(), "ShaderProgram Compiler Error", MB_ICONERROR | MB_RETRYCANCEL);
        }
    } while(retry_requested);
    ERROR_AND_DIE("Unrecoverable error. Cannot continue with malformed shader file.");
}

ID3DBlob* RHIDevice::CompileShader(const std::string& name, const void*  sourceCode, std::size_t sourceCodeSize, const std::string& entryPoint, const PipelineStage& target) const noexcept {
    unsigned int compile_options = 0;
#ifdef RENDER_DEBUG
    compile_options |= D3DCOMPILE_DEBUG;
    compile_options |= D3DCOMPILE_SKIP_OPTIMIZATION;
    compile_options |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
    compile_options |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif
#ifdef FINAL_BUILD
    compile_options |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
    compile_options |= D3DCOMPILE_SKIP_VALIDATION;
    compile_options |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
    ID3DBlob* code_blob = nullptr;
    ID3DBlob* errors = nullptr;
    std::string target_string = {};
    switch(target) {
        case PipelineStage::Vs:
            target_string = "vs_5_0";
            break;
        case PipelineStage::Hs:
            target_string = "hs_5_0";
            break;
        case PipelineStage::Ds:
            target_string = "ds_5_0";
            break;
        case PipelineStage::Gs:
            target_string = "gs_5_0";
            break;
        case PipelineStage::Ps:
            target_string = "ps_5_0";
            break;
        case PipelineStage::Cs:
            target_string = "cs_5_0";
            break;
        case PipelineStage::None:
        case PipelineStage::All:
        default:
            DebuggerPrintf("Failed to compile [%s]. Invalid PipelineStage parameter.\n", name.c_str());
            return nullptr;
    }
    HRESULT compile_hr = ::D3DCompile(  sourceCode
                                      , sourceCodeSize
                                      , name.c_str()
                                      , nullptr
                                      , D3D_COMPILE_STANDARD_FILE_INCLUDE
                                      , entryPoint.c_str()
                                      , target_string.c_str()
                                      , compile_options
                                      , 0
                                      , &code_blob
                                      , &errors);
    if(FAILED(compile_hr) || (errors != nullptr)) {
        if(errors != nullptr) {
            char* error_string = reinterpret_cast<char*>(errors->GetBufferPointer());
            DebuggerPrintf("Failed to compile [%s].  Compiler gave the following output;\n%s",
                           name.c_str(),
                           error_string);
            errors->Release();
            errors = nullptr;
        }
    }
    return code_blob;
}
