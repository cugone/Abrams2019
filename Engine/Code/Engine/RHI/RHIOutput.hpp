#pragma once

#include "Engine/RHI/RHITypes.hpp"

#include <memory>

class Window;
class RHIDevice;
class Texture;
class IntVector2;
class Rgba;
struct IDXGISwapChain4;

class RHIOutput {
public:
    RHIOutput(const RHIDevice* parent, std::unique_ptr<Window> wnd, IDXGISwapChain4* swapchain) noexcept;

    ~RHIOutput() noexcept;

    const RHIDevice* GetParentDevice() const noexcept;

    const Window* GetWindow() const noexcept;
    Window* GetWindow() noexcept;

    Texture* GetBackBuffer() noexcept;
    IntVector2 GetDimensions() const noexcept;
    float GetAspectRatio() const noexcept;

    void SetDisplayMode(const RHIOutputMode& newMode) noexcept;
    void SetDimensions(const IntVector2& clientSize) noexcept;

    void Present(bool vsync) noexcept;

protected:
    void CreateBackbuffer() noexcept;
    void ResetBackbuffer() noexcept;
    std::unique_ptr<Window> _window = nullptr;
    const RHIDevice* _parent_device = nullptr;
    std::unique_ptr<Texture> _back_buffer = nullptr;
    IDXGISwapChain4* _dxgi_swapchain = nullptr;
private:

};