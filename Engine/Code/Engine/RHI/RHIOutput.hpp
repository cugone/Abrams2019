#pragma once

#include "Engine/RHI/RHITypes.hpp"
#include "Engine/Renderer/Texture.hpp"

#include <memory>

class Window;
class RHIDevice;
class IntVector2;
class Rgba;

class RHIOutput {
public:
    RHIOutput(const RHIDevice& parent, std::unique_ptr<Window> wnd) noexcept;
    ~RHIOutput() = default;

    const RHIDevice& GetParentDevice() const noexcept;

    const Window* GetWindow() const noexcept;
    Window* GetWindow() noexcept;

    Texture* GetBackBuffer() const noexcept;
    Texture* GetDepthStencil() const noexcept;
    Texture* GetFullscreenTexture() const noexcept;

    void ResetBackbuffer() noexcept;

    IntVector2 GetDimensions() const noexcept;
    float GetAspectRatio() const noexcept;

    void SetDisplayMode(const RHIOutputMode& newMode) noexcept;
    void SetDimensions(const IntVector2& clientSize) noexcept;
    void SetTitle(const std::string& newTitle) const noexcept;

    void Present(bool vsync) noexcept;

protected:
    void CreateBuffers() noexcept;

    std::unique_ptr<Texture> CreateBackbuffer() noexcept;
    std::unique_ptr<Texture> CreateDepthStencil() noexcept;
    std::unique_ptr<Texture> CreateFullscreenTexture() noexcept;

    const RHIDevice& _parent_device;
    std::unique_ptr<Window> _window = nullptr;
    std::unique_ptr<Texture> _back_buffer = nullptr;
    std::unique_ptr<Texture> _depthstencil = nullptr;
    std::unique_ptr<Texture> _fullscreen = nullptr;

private:
};
