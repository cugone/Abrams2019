#include "Editor/Editor.hpp"

#include "Engine/Core/Image.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EngineConfig.hpp"

#include "Engine/Platform/DirectX/DirectX11FrameBuffer.hpp"
#include "Engine/Platform/PlatformUtils.hpp"
#include "Engine/Platform/Win.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Scene/Scene.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAppService.hpp"
#include "Engine/Services/IRendererService.hpp"
#include "Engine/Services/IInputService.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/UI/UISystem.hpp"

#include <algorithm>
#include <numeric>
#include <sstream>

void Editor::UpdateContentBrowserPaths(std::vector<std::filesystem::path>& cache) {
    cache.clear();
    for(const auto& p : std::filesystem::directory_iterator{m_ContentBrowserCurrentDirectory}) {
        cache.emplace_back(p.path());
    }
};

void Editor::PollContentBrowserPaths(std::vector<std::filesystem::path>& cache) {
    if(m_ContentBrowserUpdatePoll.CheckAndReset()) {
        UpdateContentBrowserPaths(cache);
    }
};

void Editor::ShowSelectedEntityComponents(TimeUtils::FPSeconds deltaSeconds) noexcept {
    /* DO NOTHING */
}

void Editor::HandleInput(TimeUtils::FPSeconds deltaSeconds) noexcept {
    HandleMenuKeyboardInput(deltaSeconds);
    HandleCameraInput(deltaSeconds);
}

bool Editor::IsSceneLoaded() const noexcept {
    return m_ActiveScene.get() != nullptr;
}

void Editor::Initialize() noexcept {
    auto& renderer = ServiceLocator::get<IRendererService>();
    m_ContentBrowserCurrentDirectory = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData);
    renderer.RegisterTexturesFromFolder(m_ContentBrowserCurrentDirectory / std::filesystem::path{"Images"}, true);
    renderer.RegisterTexturesFromFolder(m_ContentBrowserCurrentDirectory / std::filesystem::path{"Resources/Icons"}, true);
    UpdateContentBrowserPaths(m_ContentBrowserPathsCache);
    buffer = FrameBuffer::Create(FrameBufferDesc{});
}

void Editor::BeginFrame() noexcept {
    ImGui::DockSpaceOverViewport();
}

void Editor::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    ShowUI(deltaSeconds);
    HandleInput(deltaSeconds);
}

void Editor::Render() const noexcept {

    auto& renderer = ServiceLocator::get<IRendererService>();

    renderer.BeginRender(buffer->GetTexture(), Rgba::Black, buffer->GetDepthStencil());

    renderer.SetOrthoProjectionFromCamera(Camera3D{m_editorCamera.GetCamera()});
    renderer.SetCamera(m_editorCamera.GetCamera());

    const auto S = Matrix4::I;
    const auto R = Matrix4::I;
    const auto T = Matrix4::I;
    const auto M = Matrix4::MakeSRT(S, R, T);
    renderer.SetMaterial("__2D");
    renderer.DrawQuad2D(M, Rgba::Green);

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
    if(auto path = FileDialogs::OpenFile("Abrams Scene (*.abr)\0*.abr\0All Files (*.*)\0*.*\0\0"); !path.empty()) {
        
    }
}

void Editor::DoFileSaveAs() noexcept {
    if(auto path = FileDialogs::SaveFile("Abrams Scene (*.abr)\0*.abr\0All Files (*.*)\0*.*\0\0"); !path.empty()) {
    }
}

void Editor::DoFileSave() noexcept {
    /* DO NOTHING */
}

void Editor::ShowUI(TimeUtils::FPSeconds deltaSeconds) noexcept {
    ShowMainMenu(deltaSeconds);
    ShowSettingsWindow(deltaSeconds);
    ShowPropertiesWindow(deltaSeconds);
    ShowContentBrowserWindow(deltaSeconds);
    ShowMainViewport(deltaSeconds);
}

static int cameraSpeedMultiplier = 1;

void Editor::ShowMainMenu(TimeUtils::FPSeconds deltaSeconds) noexcept {
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
        if(ImGui::BeginMenu("Debug")) {
            if(ImGui::MenuItem("Dear ImGui Demo Window", nullptr, g_theUISystem->IsImguiDemoWindowVisible(), IsDebuggerAvailable())) {
                g_theUISystem->ToggleImguiDemoWindow();
            }
            if(ImGui::MenuItem("Dear ImGui Metrics Window", nullptr, g_theUISystem->IsImguiMetricsWindowVisible(), IsDebuggerAvailable())) {
                g_theUISystem->ToggleImguiMetricsWindow();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Camera")) {
            if(ImGui::BeginMenu("Camera Speed")) {
                if(ImGui::SliderInt("Speed##CameraSpeedSlider", &cameraSpeedMultiplier, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoInput)) {
                    m_editorCamera.SetSpeedMultiplier(cameraSpeedMultiplier);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Editor::ShowSettingsWindow(TimeUtils::FPSeconds deltaSeconds) noexcept {
    ImGui::Begin("Settings");
    {
        ShowSelectedEntityComponents(deltaSeconds);
    }
    ImGui::End();
}

void Editor::ShowPropertiesWindow([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    ImGui::Begin("Properties");
    {

    }
    ImGui::End();
}

void Editor::ShowMainViewport(TimeUtils::FPSeconds deltaSeconds) noexcept {
    {
        std::ostringstream ss{};
        if(IsSceneLoaded()) {
            //TODO: Scenes should store their names.
            ss << m_ActiveScene;
        } else {
            ss << "Viewport";
        }
        ImGui::Begin(ss.str().c_str(), nullptr);
    }
    const auto viewportSize = ImGui::GetContentRegionAvail();
    if(viewportSize.x != m_ViewportWidth || viewportSize.y != m_ViewportHeight) {
        m_ViewportWidth = static_cast<uint32_t>(std::floor(viewportSize.x));
        m_ViewportHeight = static_cast<uint32_t>(std::floor(viewportSize.y));
        buffer->Resize(m_ViewportWidth, m_ViewportHeight);
    }
    ImGui::Image(buffer->GetTexture(), viewportSize, Vector2::Zero, Vector2::One, Rgba::White, Rgba::NoAlpha);
    m_IsViewportWindowActive = ImGui::IsWindowFocused() || ImGui::IsWindowHovered();
    ImGui::End();
}

void Editor::ShowContentBrowserWindow(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(m_CacheNeedsImmediateUpdate) {
        UpdateContentBrowserPaths(m_ContentBrowserPathsCache);
    } else {
        PollContentBrowserPaths(m_ContentBrowserPathsCache);
    }
    ImGui::Begin("Content Browser");
    {
        auto& renderer = ServiceLocator::get<IRendererService>();
        if(m_ContentBrowserCurrentDirectory != FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData)) {
            if(ImGui::ArrowButton("Back##LEFT", ImGuiDir_Left)) {
                m_ContentBrowserCurrentDirectory = m_ContentBrowserCurrentDirectory.parent_path();
                m_CacheNeedsImmediateUpdate = true;
            }
        }
        const auto padding = 16.0f;
        const auto& config = ServiceLocator::get<IConfigService>();
        static auto scale = 0.5f;
        if(config.HasKey("UIScale")) {
            config.GetValue("UIScale", scale);
        }
        scale = std::clamp(scale, 0.125f, 2.0f);
        const auto thumbnailSize = (std::max)(32.0f, 256.0f * scale);
        const auto cellSize = thumbnailSize + padding;
        const auto panelWidth = static_cast<uint32_t>(std::floor(ImGui::GetContentRegionAvail().x));
        if(panelWidth != m_ContentBrowserPanelWidth) {
            m_ContentBrowserPanelWidth = panelWidth;
        }
        const auto columnCount = (std::min)((std::max)(1, static_cast<int>(m_ContentBrowserPanelWidth / cellSize)), 64);
        ImGui::BeginTable("##ContentBrowser", columnCount, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ContextMenuInBody);
        {
            auto id = 0u;
            for(auto& p : m_ContentBrowserPathsCache) {
                ImGui::TableNextColumn();
                const auto icon = GetAssetTextureFromType(p);
                ImGui::BeginGroup();
                ImGui::PushStyleColor(ImGuiCol_Button, Vector4::Zero);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Vector4::Zero);
                ImGui::PushID(id++);
                if(std::filesystem::is_directory(p)) {
                    if(ImGui::ImageButton(icon, Vector2{thumbnailSize, thumbnailSize}, Vector2::Zero, Vector2::One, 0, Rgba::NoAlpha, Rgba::White)) {
                        m_ContentBrowserCurrentDirectory /= p.filename();
                        m_CacheNeedsImmediateUpdate = true;
                    }
                } else {
                    ImGui::Image(icon, Vector2{thumbnailSize, thumbnailSize}, Vector2::Zero, Vector2::One, Rgba::White, Rgba::NoAlpha);
                }
                ImGui::PopID();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                const auto filename_string = p.filename().string();
                const auto filename_size = ImGui::CalcTextSize(filename_string.c_str(), nullptr);
                ImGui::TextWrapped(filename_string.c_str());
                ImGui::EndGroup();
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void Editor::HandleMenuKeyboardInput(TimeUtils::FPSeconds deltaSeconds) noexcept {
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

void Editor::HandleCameraInput(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(m_IsViewportWindowActive) {
        m_editorCamera.Update(deltaSeconds);
    }
}

bool Editor::HasAssetExtension(const std::filesystem::path& path) const noexcept {
    return std::filesystem::is_directory(path) || path.has_extension() && IsAssetExtension(path.extension());
}

Texture* Editor::GetAssetTextureFromType(const std::filesystem::path& path) const noexcept {
    auto& renderer = ServiceLocator::get<IRendererService>();
    auto* defaultTexture = renderer.GetTexture("__white");
    if(HasAssetExtension(path)) {
        const auto e = path.extension();
        std::filesystem::path p{};
        const auto BuildPath = [&](const char* pathSuffix) -> std::filesystem::path {
            auto p = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / std::filesystem::path{pathSuffix};
            p = std::filesystem::canonical(p);
            p = p.make_preferred();
            return p;
        };
        if(std::filesystem::is_directory(path)) {
            p = BuildPath("Resources/Icons/FolderAsset.png");
        } else if(e == ".txt") {
            p = BuildPath("Resources/Icons/TextAsset.png");
        } else if(e == ".ascene") {
            p = BuildPath("Resources/Icons/SceneAsset.png");
        } else if(e == ".log") {
            p = BuildPath("Resources/Icons/LogAsset.png");
        } else if(IsImageAssetExtension(e)) {
            return renderer.CreateOrGetTexture(path, IntVector3::XY_Axis);
        }
        return p.empty() ? defaultTexture : renderer.GetTexture(p.string());
    }
    return defaultTexture;
}

bool Editor::IsAssetExtension(const std::filesystem::path& ext) const noexcept {
    if(ext == ".txt") {
        return true;
    } else if(ext == ".ascene") {
        return true;
    } else if(ext == ".log") {
        return true;
    } else {
        return IsImageAssetExtension(ext);
    }
}

bool Editor::IsImageAssetExtension(const std::filesystem::path& ext) const noexcept {
    static const auto ext_string = Image::GetSupportedExtensionsList();
    static const auto ext_list = StringUtils::Split(ext_string);
    for(const auto& p : ext_list) {
        if(p == ext.string()) {
            return true;
        }
    }
    return false;
}
