#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"

#include <vector>

class Window;
class RHIDevice;

class RHIFactory {
public:
    RHIFactory();
    ~RHIFactory();

    void RestrictAltEnterToggle(const Window& window);
    IDXGISwapChain4* CreateSwapChainForHwnd(RHIDevice* device, const Window& window, const DXGI_SWAP_CHAIN_DESC1& swapchain_desc);
    IDXGIFactory6* GetDxFactory() const;
    bool QueryForAllowTearingSupport() const;
    std::vector<AdapterInfo> GetAdaptersByHighPerformancePreference() const;
    std::vector<AdapterInfo> GetAdaptersByMinimumPowerPreference() const;
    std::vector<AdapterInfo> GetAdaptersByUnspecifiedPreference() const;
protected:
private:
    IDXGIFactory6* _dxgi_factory{ nullptr };
};
