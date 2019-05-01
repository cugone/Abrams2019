#include "Engine/RHI/RHIFactory.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Engine/Rhi/RHIDevice.hpp"


RHIFactory::RHIFactory() {
#ifdef RENDER_DEBUG
    auto hr_factory = ::CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory6), reinterpret_cast<void**>(&_dxgi_factory));
#else
    auto hr_factory = ::CreateDXGIFactory2(0, __uuidof(IDXGIFactory6), reinterpret_cast<void**>(&_dxgi_factory));
#endif
    GUARANTEE_OR_DIE(SUCCEEDED(hr_factory), "Failed to create DXGIFactory6 from CreateDXGIFactory2.");
}

RHIFactory::~RHIFactory() {
    if(_dxgi_factory) {
        _dxgi_factory->Release();
        _dxgi_factory = nullptr;
    }
}

void RHIFactory::RestrictAltEnterToggle(const Window& window) {
    auto hr_mwa = _dxgi_factory->MakeWindowAssociation(window.GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);
    GUARANTEE_OR_DIE(SUCCEEDED(hr_mwa), "Failed to restrict Alt+Enter usage.");
}

IDXGISwapChain4* RHIFactory::CreateSwapChainForHwnd(RHIDevice* device, const Window& window, const DXGI_SWAP_CHAIN_DESC1& swapchain_desc) {
    IDXGISwapChain4* dxgi_swap_chain{};
    auto hr_createsc4hwnd = _dxgi_factory->CreateSwapChainForHwnd(device->GetDxDevice()
        , window.GetWindowHandle()
        , &swapchain_desc
        , nullptr
        , nullptr
        , reinterpret_cast<IDXGISwapChain1**>(&dxgi_swap_chain));
    GUARANTEE_OR_DIE(SUCCEEDED(hr_createsc4hwnd), "Failed to create swap chain.");
    return dxgi_swap_chain;
}

IDXGIFactory6* RHIFactory::GetDxFactory() const {
    return _dxgi_factory;
}

bool RHIFactory::QueryForAllowTearingSupport() const {
    BOOL allow_tearing = {};
    HRESULT hr_cfs = _dxgi_factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(allow_tearing));
    bool cfs_call_succeeded = SUCCEEDED(hr_cfs);
    if(cfs_call_succeeded) {
        return allow_tearing == TRUE;
    }
    return false;
}

std::vector<AdapterInfo> RHIFactory::GetAdaptersByHighPerformancePreference() const {
    std::vector<AdapterInfo> adapters{};
    IDXGIAdapter4* cur_adapter = nullptr;
    for(unsigned int i = 0u;
        SUCCEEDED(_dxgi_factory->EnumAdapterByGpuPreference(
            i,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            __uuidof(IDXGIAdapter4),
            reinterpret_cast<void**>(&cur_adapter)
        )
        );
        ++i) {
        AdapterInfo cur_info{};
        cur_info.adapter = cur_adapter;
        cur_adapter->GetDesc3(&cur_info.desc);
        adapters.push_back(cur_info);
    }
    return adapters;
}

std::vector<AdapterInfo> RHIFactory::GetAdaptersByMinimumPowerPreference() const {
    std::vector<AdapterInfo> adapters{};
    IDXGIAdapter4* cur_adapter = nullptr;
    for(unsigned int i = 0u;
        SUCCEEDED(_dxgi_factory->EnumAdapterByGpuPreference(
            i,
            DXGI_GPU_PREFERENCE_MINIMUM_POWER,
            __uuidof(IDXGIAdapter4),
            reinterpret_cast<void**>(&cur_adapter)
        )
        );
        ++i) {
        AdapterInfo cur_info{};
        cur_info.adapter = cur_adapter;
        cur_adapter->GetDesc3(&cur_info.desc);
        adapters.push_back(cur_info);
    }
    return adapters;
}

std::vector<AdapterInfo> RHIFactory::GetAdaptersByUnspecifiedPreference() const {
    std::vector<AdapterInfo> adapters{};
    IDXGIAdapter4* cur_adapter = nullptr;
    for(unsigned int i = 0u;
        SUCCEEDED(_dxgi_factory->EnumAdapterByGpuPreference(
            i,
            DXGI_GPU_PREFERENCE_UNSPECIFIED,
            __uuidof(IDXGIAdapter4),
            reinterpret_cast<void**>(&cur_adapter)
        )
        );
        ++i) {
        AdapterInfo cur_info{};
        cur_info.adapter = cur_adapter;
        cur_adapter->GetDesc3(&cur_info.desc);
        adapters.push_back(cur_info);
    }
    return adapters;
}
