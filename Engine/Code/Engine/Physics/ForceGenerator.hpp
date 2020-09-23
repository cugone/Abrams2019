#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include <vector>

class RigidBody;

class ForceGenerator {
public:
    virtual ~ForceGenerator() = 0;

    void attach(RigidBody* body) noexcept;
    void detach(RigidBody* body) noexcept;
    virtual void notify([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) const noexcept = 0;

protected:
    std::vector<RigidBody*> _observers{};
private:
};
