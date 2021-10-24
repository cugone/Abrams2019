#include "Editor/Editor.hpp"

#include "Engine/Platform/PlatformUtils.hpp"
#include "Engine/Platform/Win.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAppService.hpp"
#include "Engine/Services/IRendererService.hpp"
#include "Engine/Services/IInputService.hpp"

#include "Engine/Input/InputSystem.hpp"
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
            if(ImGui::MenuItem("New", "Ctrl+N")) {
                DoFileNew();
            }
            if(ImGui::MenuItem("Open...", "Ctrl+O")) {
                DoFileOpen();
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Save", "Ctrl+S", nullptr, m_ActiveScene.get())) {
                DoFileSave();
            }
            if(ImGui::MenuItem("Save As...", "", nullptr, m_ActiveScene.get())) {
                DoFileSaveAs();
            }
            if(ImGui::MenuItem("Exit")) {
                auto& app = ServiceLocator::get<IAppService>();
                app.SetIsQuitting(true);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    HandleMenuKeyboardInput();
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

void Editor::DoFileNew() noexcept {
    /* DO NOTHING */
}

void Editor::DoFileOpen() noexcept {
    if(auto path = FileDialogs::OpenFile("Abrams Scene (*.abr)\0(*.abr)\0All Files (*.*)\0(*.*)\0\0"); !path.empty()) {

    }
}

void Editor::DoFileSaveAs() noexcept {
    if(auto path = FileDialogs::SaveFile("Abrams Scene(*.abr)\0(*.abr)\0All Files (*.*)\0(*.*)\0\0"); !path.empty()) {
    }
}

void Editor::DoFileSave() noexcept {
    /* DO NOTHING */
}

void Editor::HandleMenuKeyboardInput() noexcept {
    auto& input = ServiceLocator::get<IInputService>();
    if(input.IsKeyDown(KeyCode::Ctrl)) {
        if(input.WasKeyJustPressed(KeyCode::N)) {
            DoFileNew();
        } else if(input.WasKeyJustPressed(KeyCode::O)) {
            DoFileOpen();
        } else if(input.WasKeyJustPressed(KeyCode::S)) {
            DoFileSaveAs();
        }
    }
}
