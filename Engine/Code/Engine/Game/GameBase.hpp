#pragma once

#include "Engine/Core/TimeUtils.hpp"

class GameBase {
public:
    virtual ~GameBase() noexcept {};

    virtual void Initialize() noexcept = 0;
    virtual void BeginFrame() noexcept = 0;
    virtual void Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept = 0;
    virtual void Render() const noexcept = 0;
    virtual void EndFrame() noexcept = 0;

protected:
private:
    
};
