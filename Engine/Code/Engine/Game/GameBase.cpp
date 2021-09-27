#include "Engine/Game/GameBase.hpp"

GameBase::~GameBase() noexcept {
    /* DO NOTHING */
}

void GameBase::Initialize() noexcept {
    /* DO NOTHING */
}

void GameBase::BeginFrame() noexcept {
    /* DO NOTHING */
}

void GameBase::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    /* DO NOTHING */
}

void GameBase::Render() const noexcept {
    /* DO NOTHING */
}

void GameBase::EndFrame() noexcept {
    /* DO NOTHING */
}

GameSettings& GameBase::GetSettings() noexcept {
    return defaultSettings;
}

std::weak_ptr<Scene> GameBase::GetActiveScene() const noexcept {
    return m_ActiveScene;
}
