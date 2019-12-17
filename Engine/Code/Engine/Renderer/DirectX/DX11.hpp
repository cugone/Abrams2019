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

#include <wrl/client.h>

struct AdapterInfo {
    Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter{};
    DXGI_ADAPTER_DESC3 desc{};
};

enum class AdapterPreference {
    None
    ,Unspecified = None
    ,HighPerformance
    ,MinimumPower
};

GraphicsCardDesc AdapterInfoToGraphicsCardDesc(const AdapterInfo& adapterInfo) noexcept;

struct OutputInfo {
    Microsoft::WRL::ComPtr<IDXGIOutput6> output{};
    DXGI_OUTPUT_DESC1 desc{};
};

struct DeviceInfo {
    D3D_FEATURE_LEVEL highest_supported_feature_level{};
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> dx_context{};
    Microsoft::WRL::ComPtr<ID3D11Device5> dx_device{};
};

using bitfield8_t = std::uint8_t;
using bitfield16_t = std::uint16_t;
using bitfield32_t = std::uint32_t;
using bitfield64_t = std::uint64_t;
bitfield8_t GetFilterMaskFromModes(const FilterMode& minFilterMode, const FilterMode& magFilterMode, const FilterMode& mipFilterMode, const FilterComparisonMode& minMaxComparison) noexcept;

D3D11_FILTER FilterModeToD3DFilter(const FilterMode& minFilterMode, const FilterMode& magFilterMode, const FilterMode& mipFilterMode, const FilterComparisonMode& minMaxComparison) noexcept;
FilterMode FilterModeFromString(const char* str) noexcept;
FilterMode FilterModeFromString(std::string str) noexcept;

FilterComparisonMode FilterComparisonModeFromString(const char* str) noexcept;
FilterComparisonMode FilterComparisonModeFromString(std::string str) noexcept;

D3D11_TEXTURE_ADDRESS_MODE AddressModeToD3DAddressMode(const TextureAddressMode& address_mode) noexcept;
TextureAddressMode TextureAddressModeFromString(const char* str) noexcept;
TextureAddressMode TextureAddressModeFromString(std::string str) noexcept;

D3D11_COMPARISON_FUNC ComparisonFunctionToD3DComparisonFunction(const ComparisonFunction& compareFunc) noexcept;
ComparisonFunction ComparisonFunctionFromString(std::string str) noexcept;
ComparisonFunction ComparisonFunctionFromString(const char* str) noexcept;

D3D11_STENCIL_OP StencilOperationToD3DStencilOperation(const StencilOperation& stencil_operation) noexcept;
StencilOperation StencilOperationFromString(const char* str) noexcept;
StencilOperation StencilOperationFromString(std::string str) noexcept;

D3D11_USAGE BufferUsageToD3DUsage(const BufferUsage& usage) noexcept;
D3D11_BIND_FLAG BufferBindUsageToD3DBindFlags(const BufferBindUsage& bindFlags) noexcept;
D3D11_CPU_ACCESS_FLAG CPUAccessFlagFromUsage(const BufferUsage& usage) noexcept;

D3D11_PRIMITIVE_TOPOLOGY PrimitiveTypeToD3dTopology(const PrimitiveType& topology) noexcept;

DXGI_FORMAT ImageFormatToDxgiFormat(const ImageFormat& format) noexcept;
ImageFormat DxgiFormatToImageFormat(DXGI_FORMAT format) noexcept;

D3D11_BLEND BlendFactorToD3DBlendFactor(const BlendFactor& factor) noexcept;
D3D11_BLEND_OP BlendOpToD3DBlendOp(const BlendOperation& op) noexcept;
UINT8 BlendColorWriteEnableToD3DBlendColorWriteEnable(const BlendColorWriteEnable& rt_mask) noexcept;

BlendFactor BlendFactorFromString(std::string str) noexcept;
BlendOperation BlendOperationFromString(std::string str) noexcept;
BlendColorWriteEnable BlendColorWriteEnableFromString(std::string str) noexcept;

D3D11_FILL_MODE FillModeToD3DFillMode(const FillMode& fillmode) noexcept;
D3D11_CULL_MODE CullModeToD3DCullMode(const CullMode& fillmode) noexcept;
FillMode FillModeFromString(std::string str) noexcept;
CullMode CullModeFromString(std::string str) noexcept;
WindingOrder WindingOrderFromString(std::string str) noexcept;

D3D11_RESOURCE_MISC_FLAG ResourceMiscFlagToD3DMiscFlag(const ResourceMiscFlag& flags) noexcept;

std::string PipelineStageToString(const PipelineStage& stage) noexcept;
PipelineStage PipelineStageFromString(std::string stage) noexcept;