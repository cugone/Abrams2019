#include "Engine/UI/UISystem.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/FileUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Window.hpp"

IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ImGui {
    void Image(Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) {
        ImGui::Image(reinterpret_cast<void*>(texture->GetShaderResourceView()), size, uv0, uv1, tint_col.GetRgbaAsFloats(), border_col.GetRgbaAsFloats());
    }
    bool ColorEdit3(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/) {
        auto colorAsFloats = color.GetRgbAsFloats();
        if(ImGui::ColorEdit3(label, colorAsFloats.GetAsFloatArray(), flags)) {
            color.SetRgbFromFloats(colorAsFloats);
            return true;
        }
        return false;
    }
    bool ColorEdit4(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/) {
        auto colorAsFloats = color.GetRgbaAsFloats();
        if(ImGui::ColorEdit4(label, colorAsFloats.GetAsFloatArray(), flags)) {
            color.SetRgbaFromFloats(colorAsFloats);
            return true;
        }
        return false;
    }
    bool ColorPicker3(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/) {
        auto colorAsFloats = color.GetRgbAsFloats();
        if(ImGui::ColorPicker3(label, colorAsFloats.GetAsFloatArray(), flags)) {
            color.SetRgbFromFloats(colorAsFloats);
            return true;
        }
        return false;
    }
    bool ColorPicker4(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/, Rgba* refColor /*= nullptr*/) {
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
    bool ColorButton(const char* desc_id, const Rgba& color, ImGuiColorEditFlags flags /*= 0*/, Vector2 size /*= Vector2::ZERO*/) {
        auto colorAsFloats = color.GetRgbaAsFloats();
        return ImGui::ColorButton(desc_id, colorAsFloats, flags, size);
    }

    void TextColored(const Rgba& color, const char* fmt, ...) {
        auto colorAsFloats = color.GetRgbaAsFloats();
        va_list args;
        va_start(args, fmt);
        ImGui::TextColoredV(colorAsFloats, fmt, args);
        va_end(args);
    }

}

UISystem::UISystem(Renderer* renderer)
    : EngineSubsystem()
    , _renderer(renderer)
    , _context(ImGui::CreateContext())
    , _io(&ImGui::GetIO())
{
#ifdef UI_DEBUG
#define IMGUI_DISABLE_DEMO_WINDOWS
    IMGUI_CHECKVERSION();
#endif
}

UISystem::~UISystem() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext(_context);
    _context = nullptr;
    _io = nullptr;

    _renderer = nullptr;
}

void UISystem::Initialize() {
    _io->IniFilename = nullptr;
    _io->LogFilename = nullptr;

    auto hwnd = _renderer->GetOutput()->GetWindow()->GetWindowHandle();
    auto dx_device = _renderer->GetDevice()->GetDxDevice();
    auto dx_context = _renderer->GetDeviceContext()->GetDxContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(dx_device, dx_context);

    ImGui::StyleColorsDark();

}

void UISystem::BeginFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void UISystem::Update(TimeUtils::FPSeconds /*deltaSeconds*/) {
    /* DO NOTHING */
}

void UISystem::Render() const {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UISystem::EndFrame() {
    ImGui::EndFrame();
}

bool UISystem::ProcessSystemMessage(const EngineMessage& msg) {
    return ImGui_ImplWin32_WndProcHandler(reinterpret_cast<HWND>(msg.hWnd), msg.nativeMessage, msg.wparam, msg.lparam);
}

bool UISystem::HasFocus() const {
    return _io->WantCaptureKeyboard || _io->WantCaptureMouse;
}

ImGuiIO& UISystem::GetIO() const {
    return *_io;
}
