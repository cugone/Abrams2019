#pragma once

#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Game/GameBase.hpp"

#include "Engine/Renderer/FrameBuffer.hpp"

#include <filesystem>
#include <vector>

class Texture;

class Editor : public GameBase {
public:
    void Initialize() noexcept override;
    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;

    const GameSettings& GetSettings() const noexcept override;
    GameSettings& GetSettings() noexcept override;

protected:
private:
    void DoFileNew() noexcept;
    void DoFileOpen() noexcept;
    void DoFileSaveAs() noexcept;
    void DoFileSave() noexcept;

    void ShowMainImage() noexcept;
    void ShowContentBrowserWindow() noexcept;
    void HandleMenuKeyboardInput() noexcept;

    bool HasAssetExtension(const std::filesystem::path& path) const noexcept;
    bool IsAssetExtension(const std::filesystem::path& ext) const noexcept;
    Texture* GetAssetTextureFromType(const std::filesystem::path& path) const noexcept;


    void UpdateContentBrowserPaths(std::vector<std::filesystem::path>& cache);
    void poll_paths(std::vector<std::filesystem::path>& cache);

    std::filesystem::path m_ContentBrowserCurrentDirectory{};

    Stopwatch m_ContentBrowserUpdatePoll{1.0f};
    std::vector<std::filesystem::path> m_ContentBrowserPathsCache{};
    bool m_CacheNeedsImmediateUpdate{true};
    //std::unique_ptr<FrameBuffer> buffer{};
};
