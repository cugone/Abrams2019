#include "Editor/ContentBrowserPanel.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IConfigService.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Editor/Editor.hpp"

void ContentBrowserPanel::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(m_CacheNeedsImmediateUpdate) {
        UpdateContentBrowserPaths();
    } else {
        PollContentBrowserPaths();
    }
    ImGui::Begin("Content Browser");
    {
        if(currentDirectory != FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData)) {
            if(ImGui::ArrowButton("Back##LEFT", ImGuiDir_Left)) {
                currentDirectory = currentDirectory.parent_path();
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
        if(panelWidth != m_PanelWidth) {
            m_PanelWidth = panelWidth;
        }
        const auto columnCount = (std::min)((std::max)(1, static_cast<int>(m_PanelWidth / cellSize)), 64);
        ImGui::BeginTable("##ContentBrowser", columnCount, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ContextMenuInBody);
        {
            auto id = 0u;
            for(auto& p : m_PathsCache) {
                ImGui::TableNextColumn();
                const auto* editor = GetGameAs<Editor>();
                const auto icon = editor->GetAssetTextureFromType(p);
                ImGui::BeginGroup();
                ImGui::PushStyleColor(ImGuiCol_Button, Vector4::Zero);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Vector4::Zero);
                ImGui::PushID(id++);
                if(std::filesystem::is_directory(p)) {
                    if(ImGui::ImageButton(icon, Vector2{thumbnailSize, thumbnailSize}, Vector2::Zero, Vector2::One, 0, Rgba::NoAlpha, Rgba::White)) {
                        currentDirectory /= p.filename();
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

void ContentBrowserPanel::UpdateContentBrowserPaths() noexcept {
    m_PathsCache.clear();
    for(const auto& p : std::filesystem::directory_iterator{currentDirectory}) {
        m_PathsCache.emplace_back(p.path());
    }
}

void ContentBrowserPanel::PollContentBrowserPaths() noexcept {
    if(m_UpdatePoll.CheckAndReset()) {
        UpdateContentBrowserPaths();
    }
}
