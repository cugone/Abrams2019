#pragma once

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/EngineSubsystem.hpp"

#include "Engine/Renderer/Camera2D.hpp"

#ifndef UI_DEBUG
    #define IMGUI_DISABLE_DEMO_WINDOWS
#else
    #undef IMGUI_DISABLE_DEMO_WINDOWS
#endif

#include "Thirdparty/Imgui/imgui.h"
#include "Thirdparty/Imgui/imgui_impl_dx11.h"
#include "Thirdparty/Imgui/imgui_impl_win32.h"
#include "Thirdparty/Imgui/imgui_stdlib.h"

#include <map>
#include <memory>
#include <filesystem>

namespace UI {
    class Widget;
}
class Renderer;
class FileLogger;
class InputSystem;

class UISystem : public EngineSubsystem {
public:
    UISystem() = delete;
    explicit UISystem(FileLogger& fileLogger, Renderer& renderer, InputSystem& inputSystem) noexcept;
    UISystem(const UISystem& other) = default;
    UISystem(UISystem&& other) = default;
    UISystem& operator=(const UISystem& other) = default;
    UISystem& operator=(UISystem&& other) = default;
    virtual ~UISystem() noexcept;

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(TimeUtils::FPSeconds deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    [[nodiscard]] virtual bool ProcessSystemMessage(const EngineMessage& msg) noexcept override;

    [[nodiscard]] bool HasFocus() const noexcept;

    [[nodiscard]] bool WantsInputCapture() const noexcept;
    [[nodiscard]] bool WantsInputKeyboardCapture() const noexcept;
    [[nodiscard]] bool WantsInputMouseCapture() const noexcept;

    [[nodiscard]] bool IsImguiDemoWindowVisible() const noexcept;
    void ToggleImguiDemoWindow() noexcept;

    void LoadUiWidgetsFromFolder(std::filesystem::path path, bool recursive = false);
    void LoadUiWidget(const std::string& name);
    void UnloadUiWidget(const std::string& name);

    void AddUiWidgetToViewport(UI::Widget& widget);
    void RemoveUiWidgetFromViewport(UI::Widget& widget);
    [[nodiscard]] UI::Widget* GetWidgetByName(const std::string& nameOrFilepath) const;
    void RegisterUiWidgetsFromFolder(std::filesystem::path folderpath, bool recursive = false);

protected:
private:
    
    [[nodiscard]] bool IsWidgetLoaded(const UI::Widget& widget) const noexcept;

    FileLogger& _fileLogger;
    Renderer& _renderer;
    InputSystem& _inputSystem;
    ImGuiContext* _context{};
    mutable Camera2D _ui_camera{};
    std::map<std::string, std::unique_ptr<UI::Widget>> _widgets{};
    std::vector<UI::Widget*> _active_widgets{};
    std::filesystem::path _ini_filepath{"Engine/Config/ui.ini"};
    std::filesystem::path _log_filepath{"Engine/Config/ui.log"};
    bool show_imgui_demo_window = false;
};

class Texture;
class Rgba;
class Vector2;
class Vector4;
//Custom ImGui overloads
namespace ImGui {
void Image(const Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) noexcept;
void Image(Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) noexcept;
[[nodiscard]] bool ColorEdit3(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0) noexcept;
[[nodiscard]] bool ColorEdit4(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0) noexcept;
[[nodiscard]] bool ColorPicker3(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0) noexcept;
[[nodiscard]] bool ColorPicker4(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0, Rgba* refColor = nullptr) noexcept;
[[nodiscard]] bool ColorButton(const char* desc_id, Rgba& color, ImGuiColorEditFlags flags = 0, Vector2 size = Vector2::ZERO) noexcept;
void TextColored(const Rgba& color, const char* fmt, ...) noexcept;
} // namespace ImGui
