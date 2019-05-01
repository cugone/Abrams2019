#pragma once

#ifdef _DEBUG
#define INITGUID
#endif

#include <d3d11_4.h>
#include <dxgi1_6.h>

// DEBUG STUFF
#include <dxgidebug.h>
#include <D3Dcommon.h>
#include <d3d11sdklayers.h>

// LIBS
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )
#pragma comment( lib, "dxguid.lib" )

#include <d3d11shader.h>
#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )

#include "Engine/RHI/RHITypes.hpp"

#include <string>
#include <cstdint>

struct AdapterInfo {
    IDXGIAdapter4* adapter = nullptr;
    DXGI_ADAPTER_DESC3 desc{};
    void Release() {
        if(adapter) {
            adapter->Release();
            adapter = nullptr;
        }
    }
};

GraphicsCardDesc AdapterInfoToGraphicsCardDesc(const AdapterInfo& adapterInfo);

struct OutputInfo {
    IDXGIOutput6* output = nullptr;
    DXGI_OUTPUT_DESC1 desc{};
};

struct DeviceInfo {
    D3D_FEATURE_LEVEL highest_supported_feature_level{};
    ID3D11DeviceContext* dx_context = nullptr;
    ID3D11Device5* dx_device = nullptr;
};

using bitfield8_t = std::uint8_t;
using bitfield16_t = std::uint16_t;
using bitfield32_t = std::uint32_t;
using bitfield64_t = std::uint64_t;
bitfield8_t GetFilterMaskFromModes(const FilterMode& minFilterMode, const FilterMode& magFilterMode, const FilterMode& mipFilterMode, const FilterComparisonMode& minMaxComparison);

D3D11_FILTER FilterModeToD3DFilter(const FilterMode& minFilterMode, const FilterMode& magFilterMode, const FilterMode& mipFilterMode, const FilterComparisonMode& minMaxComparison);
FilterMode FilterModeFromString(const char* str);
FilterMode FilterModeFromString(std::string str);

FilterComparisonMode FilterComparisonModeFromString(const char* str);
FilterComparisonMode FilterComparisonModeFromString(std::string str);

D3D11_TEXTURE_ADDRESS_MODE AddressModeToD3DAddressMode(const TextureAddressMode& address_mode);
TextureAddressMode TextureAddressModeFromString(const char* str);
TextureAddressMode TextureAddressModeFromString(std::string str);

D3D11_COMPARISON_FUNC ComparisonFunctionToD3DComparisonFunction(const ComparisonFunction& compareFunc);
ComparisonFunction ComparisonFunctionFromString(std::string str);
ComparisonFunction ComparisonFunctionFromString(const char* str);

D3D11_STENCIL_OP StencilOperationToD3DStencilOperation(const StencilOperation& stencil_operation);
StencilOperation StencilOperationFromString(const char* str);
StencilOperation StencilOperationFromString(std::string str);

D3D11_USAGE BufferUsageToD3DUsage(const BufferUsage& usage);
D3D11_BIND_FLAG BufferBindUsageToD3DBindFlags(const BufferBindUsage& bindFlags);
D3D11_CPU_ACCESS_FLAG CPUAccessFlagFromUsage(const BufferUsage& usage);

D3D11_PRIMITIVE_TOPOLOGY PrimitiveTypeToD3dTopology(const PrimitiveType& topology);

DXGI_FORMAT ImageFormatToDxgiFormat(const ImageFormat& format);
ImageFormat DxgiFormatToImageFormat(DXGI_FORMAT format);

D3D11_BLEND BlendFactorToD3DBlendFactor(const BlendFactor& factor);
D3D11_BLEND_OP BlendOpToD3DBlendOp(const BlendOperation& op);
UINT8 BlendColorWriteEnableToD3DBlendColorWriteEnable(const BlendColorWriteEnable& rt_mask);

BlendFactor BlendFactorFromString(std::string str);
BlendOperation BlendOperationFromString(std::string str);
BlendColorWriteEnable BlendColorWriteEnableFromString(std::string str);

D3D11_FILL_MODE FillModeToD3DFillMode(const FillMode& fillmode);
D3D11_CULL_MODE CullModeToD3DCullMode(const CullMode& fillmode);
FillMode FillModeFromString(std::string str);
CullMode CullModeFromString(std::string str);

D3D11_RESOURCE_MISC_FLAG ResourceMiscFlagToD3DMiscFlag(const ResourceMiscFlag& flags);

std::string PipelineStageToString(const PipelineStage& stage);
PipelineStage PipelineStageFromString(std::string stage);