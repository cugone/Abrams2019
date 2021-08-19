#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Game/GameSettings.hpp"

class GameBase {
public:
    GameBase() noexcept = default;
    GameBase(const GameBase& other) = default;
    GameBase(GameBase&& other) = default;
    GameBase& operator=(const GameBase& other) = default;
    GameBase& operator=(GameBase&& other) = default;
    virtual ~GameBase() noexcept = 0;

    virtual void Initialize() noexcept;
    virtual void BeginFrame() noexcept;
    virtual void Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept;
    virtual void Render() const noexcept;
    virtual void EndFrame() noexcept;

    virtual GameSettings& GetSettings() noexcept;

protected:
    static inline GameSettings defaultSettings{};
private:
    
};
