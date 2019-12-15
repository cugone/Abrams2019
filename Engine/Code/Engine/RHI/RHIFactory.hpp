#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"

#include <vector>

class Window;
class RHIDevice;

class RHIFactory {
public:
    RHIFactory() noexcept;
    ~RHIFactory() noexcept;

    void RestrictAltEnterToggle(const RHIDevice& device) noexcept;
    IDXGISwapChain4* CreateSwapChainForHwnd(RHIDevice* device, const Window& window, const DXGI_SWAP_CHAIN_DESC1& swapchain_desc) noexcept;

    IDXGIFactory6* GetDxFactory() const noexcept;
    bool QueryForAllowTearingSupport() const noexcept;

    std::vector<AdapterInfo> GetAdaptersByPreference(const AdapterPreference& preference) const noexcept;
    std::vector<AdapterInfo> GetAdaptersByHighPerformancePreference() const noexcept;
    std::vector<AdapterInfo> GetAdaptersByMinimumPowerPreference() const noexcept;
    std::vector<AdapterInfo> GetAdaptersByUnspecifiedPreference() const noexcept;
protected:
private:
    IDXGIFactory6* _dxgi_factory{ nullptr };
};
