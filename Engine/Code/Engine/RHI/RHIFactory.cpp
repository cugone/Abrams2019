#include "Engine/RHI/RHIFactory.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Engine/Rhi/RHIDevice.hpp"


RHIFactory::RHIFactory() noexcept {
#ifdef RENDER_DEBUG
    auto hr_factory = ::CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory6), &_dxgi_factory);
#else
    auto hr_factory = ::CreateDXGIFactory2(0, __uuidof(IDXGIFactory6), &_dxgi_factory);
#endif
    GUARANTEE_OR_DIE(SUCCEEDED(hr_factory), "Failed to create DXGIFactory6 from CreateDXGIFactory2.");
}

void RHIFactory::RestrictAltEnterToggle(const RHIDevice& device) noexcept {
    namespace MWRL = Microsoft::WRL;
    HWND hwnd{};
    auto got_hwnd = device.GetDxSwapChain()->GetHwnd(&hwnd);
    GUARANTEE_OR_DIE(SUCCEEDED(got_hwnd), "Failed to get Hwnd for restricting Alt+Enter usage.");
    MWRL::ComPtr<IDXGIFactory6> factory{};
    auto got_parent = device.GetDxSwapChain()->GetParent(__uuidof(IDXGIFactory6), &factory);
    GUARANTEE_OR_DIE(SUCCEEDED(got_parent), "Failed to get parent factory for restricting Alt+Enter usage.");
    auto hr_mwa = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES);
    GUARANTEE_OR_DIE(SUCCEEDED(hr_mwa), "Failed to restrict Alt+Enter usage.");
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> RHIFactory::CreateSwapChainForHwnd(const RHIDevice& device, const Window& window, const DXGI_SWAP_CHAIN_DESC1& swapchain_desc) noexcept {
    namespace MWRL = Microsoft::WRL;
    MWRL::ComPtr<IDXGISwapChain1> swap_chain1{};
    MWRL::ComPtr<IDXGISwapChain4> swap_chain4{};
    auto hr_createsc4hwnd = _dxgi_factory->CreateSwapChainForHwnd(device.GetDxDevice()
        , window.GetWindowHandle()
        , &swapchain_desc
        , nullptr
        , nullptr
        , &swap_chain1);
    const auto hr_create = StringUtils::FormatWindowsMessage(hr_createsc4hwnd);
    GUARANTEE_OR_DIE(SUCCEEDED(hr_createsc4hwnd), hr_create.c_str());
    auto hr_dxgisc4 = swap_chain1.As(&swap_chain4);
    const auto hr_error = StringUtils::FormatWindowsMessage(hr_dxgisc4);
    GUARANTEE_OR_DIE(SUCCEEDED(hr_dxgisc4), hr_error.c_str());
    return swap_chain4;
}

bool RHIFactory::QueryForAllowTearingSupport(const RHIDevice& device) const noexcept {
    BOOL allow_tearing = {};
    Microsoft::WRL::ComPtr<IDXGIFactory6> factory{};
    auto got_parent = device.GetDxSwapChain()->GetParent(__uuidof(IDXGIFactory6), &factory);
    GUARANTEE_OR_DIE(SUCCEEDED(got_parent), "Failed to get parent factory when querying for AllowTearingSupport.");
    HRESULT hr_cfs = factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(allow_tearing));
    factory.Reset();
    if(FAILED(hr_cfs)) {
        const auto str = StringUtils::FormatWindowsMessage(hr_cfs);
        DebuggerPrintf(str.c_str());
        return false;
    }
    return allow_tearing == TRUE;
}


std::vector<AdapterInfo> RHIFactory::GetAdaptersByPreference(const AdapterPreference& preference) const noexcept {
    switch(preference) {
    case AdapterPreference::HighPerformance:
        return GetAdaptersByHighPerformancePreference();
    case AdapterPreference::MinimumPower:
        return GetAdaptersByMinimumPowerPreference();
    case AdapterPreference::Unspecified: [[fallthrough]];
    /*case AdapterPreference::None: [[fallthrough]]; Also Unspecified */
    default:
        return GetAdaptersByUnspecifiedPreference();
    }
}

std::vector<AdapterInfo> RHIFactory::GetAdaptersByHighPerformancePreference() const noexcept {
    std::vector<AdapterInfo> adapters{};
    Microsoft::WRL::ComPtr<IDXGIAdapter4> cur_adapter{};
    for(unsigned int i = 0u;
        SUCCEEDED(_dxgi_factory->EnumAdapterByGpuPreference(
            i,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            __uuidof(IDXGIAdapter4),
            &cur_adapter
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

std::vector<AdapterInfo> RHIFactory::GetAdaptersByMinimumPowerPreference() const noexcept {
    std::vector<AdapterInfo> adapters{};
    Microsoft::WRL::ComPtr<IDXGIAdapter4> cur_adapter{};
    for(unsigned int i = 0u;
        SUCCEEDED(_dxgi_factory->EnumAdapterByGpuPreference(
            i,
            DXGI_GPU_PREFERENCE_MINIMUM_POWER,
            __uuidof(IDXGIAdapter4),
            &cur_adapter
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

std::vector<AdapterInfo> RHIFactory::GetAdaptersByUnspecifiedPreference() const noexcept {
    std::vector<AdapterInfo> adapters{};
    Microsoft::WRL::ComPtr<IDXGIAdapter4> cur_adapter{};
    for(unsigned int i = 0u;
        SUCCEEDED(_dxgi_factory->EnumAdapterByGpuPreference(
            i,
            DXGI_GPU_PREFERENCE_UNSPECIFIED,
            __uuidof(IDXGIAdapter4),
            &cur_adapter
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
