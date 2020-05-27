#include "Engine/UI/UISystem.hpp"

#include "Engine/Core/FileLogger.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/KerningFont.hpp"

#include "Engine/Profiling/ProfileLogScope.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Engine/UI/Widget.hpp"

#include <algorithm>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ImGui {
void Image(const Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) noexcept {
    ImGui::Image(reinterpret_cast<void*>(texture->GetShaderResourceView()), size, uv0, uv1, tint_col.GetRgbaAsFloats(), border_col.GetRgbaAsFloats());
}
void Image(Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) noexcept {
    ImGui::Image(reinterpret_cast<void*>(texture->GetShaderResourceView()), size, uv0, uv1, tint_col.GetRgbaAsFloats(), border_col.GetRgbaAsFloats());
}
bool ColorEdit3(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/) noexcept {
    auto colorAsFloats = color.GetRgbAsFloats();
    if(ImGui::ColorEdit3(label, colorAsFloats.GetAsFloatArray(), flags)) {
        color.SetRgbFromFloats(colorAsFloats);
        return true;
    }
    return false;
}
bool ColorEdit4(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/) noexcept {
    auto colorAsFloats = color.GetRgbaAsFloats();
    if(ImGui::ColorEdit4(label, colorAsFloats.GetAsFloatArray(), flags)) {
        color.SetRgbaFromFloats(colorAsFloats);
        return true;
    }
    return false;
}
bool ColorPicker3(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/) noexcept {
    auto colorAsFloats = color.GetRgbAsFloats();
    if(ImGui::ColorPicker3(label, colorAsFloats.GetAsFloatArray(), flags)) {
        color.SetRgbFromFloats(colorAsFloats);
        return true;
    }
    return false;
}
bool ColorPicker4(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/, Rgba* refColor /*= nullptr*/) noexcept {
    auto colorAsFloats = color.GetRgbaAsFloats();
    Vector4 refColorAsFloats{};
    if(refColor) {
        refColorAsFloats = refColor->GetRgbaAsFloats();
    }
    if(ImGui::ColorPicker4(label, colorAsFloats.GetAsFloatArray(), flags, refColor ? refColorAsFloats.GetAsFloatArray() : nullptr)) {
        color.SetRgbaFromFloats(colorAsFloats);
        if(refColor) {
            refColor->SetRgbaFromFloats(refColorAsFloats);
        }
        return true;
    }
    return false;
}
bool ColorButton(const char* desc_id, const Rgba& color, ImGuiColorEditFlags flags /*= 0*/, Vector2 size /*= Vector2::ZERO*/) noexcept {
    auto colorAsFloats = color.GetRgbaAsFloats();
    return ImGui::ColorButton(desc_id, colorAsFloats, flags, size);
}

void TextColored(const Rgba& color, const char* fmt, ...) noexcept {
    auto colorAsFloats = color.GetRgbaAsFloats();
    va_list args;
    va_start(args, fmt);
    ImGui::TextColoredV(colorAsFloats, fmt, args);
    va_end(args);
}

} // namespace ImGui

UISystem::UISystem(FileLogger& fileLogger, Renderer& renderer) noexcept
: EngineSubsystem()
, _fileLogger(fileLogger)
, _renderer(renderer)
, _context(ImGui::CreateContext())
, _io(&ImGui::GetIO()) {
#ifdef UI_DEBUG
    IMGUI_CHECKVERSION();
#endif
}

UISystem::~UISystem() noexcept {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext(_context);
    _context = nullptr;
    _io = nullptr;

    _widgets.clear();

}

void UISystem::Initialize() {
    _io->IniFilename = nullptr;
    _io->LogFilename = nullptr;

    auto* hwnd = _renderer.GetOutput()->GetWindow()->GetWindowHandle();
    auto* dx_device = _renderer.GetDevice()->GetDxDevice();
    auto* dx_context = _renderer.GetDeviceContext()->GetDxContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(dx_device, dx_context);
    const auto dims = Vector2{_renderer.GetOutput()->GetDimensions()};
    _io->DisplaySize.x = dims.x;
    _io->DisplaySize.y = dims.y;
    ImGui::StyleColorsDark();

}

void UISystem::BeginFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void UISystem::Update(TimeUtils::FPSeconds /*deltaSeconds*/) {
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
    if(show_imgui_demo_window) {
        ImGui::ShowDemoWindow(&show_imgui_demo_window);
    }
#endif
}

void UISystem::Render() const {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        //2D View / HUD
        const float ui_view_height = _renderer.GetCurrentViewport().height;
        const float ui_view_width = ui_view_height * _ui_camera.GetAspectRatio();
        const auto ui_view_extents = Vector2{ui_view_width, ui_view_height};
        const auto ui_view_half_extents = ui_view_extents * 0.5f;
        auto ui_leftBottom = Vector2{-ui_view_half_extents.x, ui_view_half_extents.y};
        auto ui_rightTop = Vector2{ui_view_half_extents.x, -ui_view_half_extents.y};
        auto ui_nearFar = Vector2{0.0f, 1.0f};
        auto ui_cam_pos = ui_view_half_extents;
        _ui_camera.position = ui_cam_pos;
        _ui_camera.orientation_degrees = 0.0f;
        _ui_camera.SetupView(ui_leftBottom, ui_rightTop, ui_nearFar, _renderer.GetCurrentViewportAspectRatio());
        _renderer.SetCamera(_ui_camera);

        for(const auto* cur_widget : _active_widgets) {
            cur_widget->Render();
        }
#if defined(RENDER_DEBUG)
        for(const auto* cur_widget : _active_widgets) {
            cur_widget->DebugRender();
        }
#endif

}

void UISystem::EndFrame() {
    ImGui::EndFrame();
}

bool UISystem::ProcessSystemMessage(const EngineMessage& msg) noexcept {
    return ImGui_ImplWin32_WndProcHandler(reinterpret_cast<HWND>(msg.hWnd), msg.nativeMessage, msg.wparam, msg.lparam);
}

bool UISystem::HasFocus() const noexcept {
    return _io->WantCaptureKeyboard || _io->WantCaptureMouse;
}

ImGuiIO& UISystem::GetIO() const noexcept {
    return *_io;
}

bool UISystem::WantsInputCapture() const noexcept {
    return WantsInputKeyboardCapture() || WantsInputMouseCapture();
}

bool UISystem::WantsInputKeyboardCapture() const noexcept {
    return GetIO().WantCaptureKeyboard;
}

bool UISystem::WantsInputMouseCapture() const noexcept {
    return GetIO().WantCaptureMouse;
}

void UISystem::ToggleImguiDemoWindow() noexcept {
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
    show_imgui_demo_window = !show_imgui_demo_window;
#endif
}

void UISystem::RegisterUiWidgetsFromFolder(std::filesystem::path folderpath, bool recursive /*= false*/) {
    const auto widgets_lambda = [this](const std::filesystem::path& path) {
        auto newWidget = std::make_unique<UI::Widget>(_renderer, path);
        const auto name = newWidget->name;
        _widgets.try_emplace(name, std::move(newWidget));
    };
    FileUtils::ForEachFileInFolder(folderpath, ".ui", widgets_lambda, recursive);
}

bool UISystem::IsWidgetLoaded(const UI::Widget& widget) const noexcept {
    return std::find(std::begin(_active_widgets), std::end(_active_widgets), &widget) != std::end(_active_widgets);
}

void UISystem::LoadUiWidgetsFromFolder(std::filesystem::path path, bool recursive /*= false*/) {
    const auto widgets_lambda = [this](const std::filesystem::path& path) {
        if(tinyxml2::XMLDocument doc; tinyxml2::XML_SUCCESS == doc.LoadFile(path.string().c_str())) {
            if(const auto* root = doc.RootElement(); DataUtils::HasAttribute(*root, "name")) {
                if(const auto name = DataUtils::ParseXmlAttribute(*root, "name", ""); !name.empty()) {
                    LoadUiWidget(name);
                }
            }
        }
    };
    FileUtils::ForEachFileInFolder(path, ".ui", widgets_lambda, recursive);
}

void UISystem::LoadUiWidget(const std::string& name) {
    if(auto* widget = GetWidgetByName(name)) {
        _active_widgets.push_back(widget);
    }
}

void UISystem::UnloadUiWidget(const std::string& name) {
    _active_widgets.erase(std::remove_if(std::begin(_active_widgets), std::end(_active_widgets), [&name](UI::Widget* widget) { return widget->name == name; }), std::end(_active_widgets));
}

void UISystem::AddUiWidgetToViewport(UI::Widget& widget) {
    const auto viewport = _renderer.GetCurrentViewport();
    const auto viewportDims = Vector2{viewport.width, viewport.height};
    if(!IsWidgetLoaded(widget)) {
        LoadUiWidget(widget.name);
    }
}

void UISystem::RemoveUiWidgetFromViewport(UI::Widget& widget) {
    UnloadUiWidget(widget.name);
}

UI::Widget* UISystem::GetWidgetByName(const std::string& name) const {
    if(const auto& found = _widgets.find(name); found != std::end(_widgets)) {
        return found->second.get();
    }
    return nullptr;
}
