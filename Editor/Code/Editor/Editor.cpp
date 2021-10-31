#include "Editor/Editor.hpp"

#include "Engine/Core/Image.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EngineConfig.hpp"

#include "Engine/Platform/PlatformUtils.hpp"
#include "Engine/Platform/Win.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAppService.hpp"
#include "Engine/Services/IRendererService.hpp"
#include "Engine/Services/IInputService.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/UI/UISystem.hpp"

#include <algorithm>
#include <numeric>

void Editor::UpdateContentBrowserPaths(std::vector<std::filesystem::path>& cache) {
    cache.clear();
    for(const auto& p : std::filesystem::directory_iterator{m_ContentBrowserCurrentDirectory}) {
        cache.emplace_back(p.path());
    }
};

void Editor::poll_paths(std::vector<std::filesystem::path>& cache) {
    if(m_ContentBrowserUpdatePoll.CheckAndReset()) {
        UpdateContentBrowserPaths(cache);
    }
};


void Editor::Initialize() noexcept {
    auto& renderer = ServiceLocator::get<IRendererService>();
    m_ContentBrowserCurrentDirectory = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData);
    renderer.RegisterTexturesFromFolder(m_ContentBrowserCurrentDirectory / std::filesystem::path{"Images"}, true);
    renderer.RegisterTexturesFromFolder(m_ContentBrowserCurrentDirectory / std::filesystem::path{"Icons"}, true);
    UpdateContentBrowserPaths(m_ContentBrowserPathsCache);
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
    ShowContentBrowserWindow();
    ShowMainImage();
    HandleMenuKeyboardInput();
}

void Editor::Render() const noexcept {

    auto& renderer = ServiceLocator::get<IRendererService>();

    //renderer.BeginRender(buffer->GetTexture(), Rgba::Black, buffer->GetDepthStencil());

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

void Editor::ShowMainImage() noexcept {
    //ImGui::Begin("SceneViewport", nullptr, ImGuiWindowFlags_NoTitleBar);
    //ImGui::Image(buffer->GetTexture(), ImGui::GetWindowSize(), Vector2::Zero, Vector2::One, Rgba::NoAlpha, Rgba::NoAlpha);
    //ImGui::End();
}

void Editor::ShowContentBrowserWindow() noexcept {
    if(m_CacheNeedsImmediateUpdate) {
        UpdateContentBrowserPaths(m_ContentBrowserPathsCache);
    } else {
        poll_paths(m_ContentBrowserPathsCache);
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
        //TODO: Add global UI scaling
        static auto scale = 1.0f;
        scale = std::clamp(scale, 0.125f, 2.0f);
        const auto thumbnailSize = std::max(32.0f, 256.0f * scale);
        const auto cellSize = thumbnailSize + padding;
        const auto panelWidth = ImGui::GetContentRegionAvail().x;
        const auto columnCount = std::min(std::max(1, static_cast<int>(panelWidth / cellSize)), 64);
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
                ImGui::TextWrapped(p.filename().string().c_str());
                ImGui::EndGroup();
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();
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
        const auto IsImageAsset = [&](const std::filesystem::path& ext) {
            for(const auto& p : StringUtils::Split((Image::GetSupportedExtensionsList()))) {
                if(p == ext.string()) return true;
            }
            return false;
        };
        if(std::filesystem::is_directory(path)) {
            p = BuildPath("Icons/FolderAsset.png");
        } else if(e == ".txt") {
            p = BuildPath("Icons/TextAsset.png");
        } else if(e == ".ascene") {
            p = BuildPath("Icons/SceneAsset.png");
        } else if(e == ".log") {
            p = BuildPath("Icons/LogAsset.png");
        } else if(IsImageAsset(e)) {
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
        const auto& supportedExtensions = StringUtils::Split(Image::GetSupportedExtensionsList());
        if(std::find(std::cbegin(supportedExtensions), std::cend(supportedExtensions), ext.string()) != std::cend(supportedExtensions)) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}
