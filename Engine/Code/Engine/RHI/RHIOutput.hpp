#pragma once

#include "Engine/RHI/RHITypes.hpp"

class Window;
class RHIDevice;
class Texture;
class IntVector2;
class Rgba;
struct IDXGISwapChain4;

class RHIOutput {
public:
    RHIOutput(const RHIDevice* parent, Window* wnd, IDXGISwapChain4* swapchain);

    ~RHIOutput();

    const RHIDevice* GetParentDevice() const;
    RHIDevice* GetParentDevice();

    const Window* GetWindow() const;
    Window* GetWindow();

    Texture* GetBackBuffer();
    IntVector2 GetDimensions() const;
    float GetAspectRatio() const;

    void SetDisplayMode(const RHIOutputMode& newMode);
    void SetDimensions(const IntVector2& clientSize);

    void Present(bool vsync);

protected:
    void CreateBackbuffer();
    void ResetBackbuffer();
    Window * _window = nullptr;
    const RHIDevice* _parent_device = nullptr;
    Texture* _back_buffer = nullptr;
    IDXGISwapChain4* _dxgi_swapchain = nullptr;
private:

};