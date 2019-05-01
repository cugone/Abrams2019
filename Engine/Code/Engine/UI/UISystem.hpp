#pragma once

#include "Engine/Core/EngineSubsystem.hpp"

#include "Thirdparty/Imgui/imgui.h"
#include "Thirdparty/Imgui/imgui_impl_dx11.h"
#include "Thirdparty/Imgui/imgui_impl_win32.h"
#include "Thirdparty/Imgui/imgui_stdlib.h"

class Renderer;

class UISystem : public EngineSubsystem {
public:
    explicit UISystem(Renderer* renderer);
    UISystem(const UISystem& other) = default;
    UISystem(UISystem&& other) = default;
    UISystem& operator=(const UISystem& other) = default;
    UISystem& operator=(UISystem&& other) = default;
    virtual ~UISystem();

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(TimeUtils::FPSeconds deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

    bool HasFocus() const;

    ImGuiIO& GetIO() const;
protected:
private:
    Renderer* _renderer{};
    ImGuiContext* _context{};
    ImGuiIO* _io{};
};

class Texture;
class Rgba;
class Vector2;
class Vector4;
//Custom ImGui overloads
namespace ImGui {
    void Image(Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col);
    bool ColorEdit3(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0);
    bool ColorEdit4(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0);
    bool ColorPicker3(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0);
    bool ColorPicker4(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0, Rgba* refColor = nullptr);
    bool ColorButton(const char* desc_id, Rgba& color, ImGuiColorEditFlags flags = 0, Vector2 size = Vector2::ZERO);
    void TextColored(const Rgba& color, const char* fmt, ...);
}
