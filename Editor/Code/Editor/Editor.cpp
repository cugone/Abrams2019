#include "Editor/Editor.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAppService.hpp"
#include "Engine/Services/IRendererService.hpp"
#include "Engine/UI/UISystem.hpp"

void Editor::Initialize() noexcept {
    /* DO NOTHING */
}

void Editor::BeginFrame() noexcept {
    ImGui::DockSpaceOverViewport();
}

void Editor::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    ImGui::BeginMainMenuBar();
    {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Exit")) {
                auto& app = ServiceLocator::get<IAppService>();
                app.SetIsQuitting(true);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Editor::Render() const noexcept {
    auto& renderer = ServiceLocator::get<IRendererService>();
    renderer.BeginRenderToBackbuffer();

}

void Editor::EndFrame() noexcept {
    /* DO NOTHING */
}

const GameSettings& Editor::GetSettings() const noexcept {
    return GameBase::GetSettings();
}

GameSettings& Editor::GetSettings() noexcept {
    return GameBase::GetSettings();
}
