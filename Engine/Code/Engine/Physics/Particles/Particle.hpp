#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/Material.hpp"

class Particle {
public:
    Particle() = default;
    Particle(const Particle& other) = default;
    Particle(Particle&& other) = default;
    Particle& operator=(const Particle& other) = default;
    Particle& operator=(Particle&& other) = default;
    ~Particle() = default;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

protected:
private:
    Vector3 position{};
    Vector3 velocity{};
    TimeUtils::FPSeconds lifetime{};
    Rgba color{};
    Material* material{};
};
