#pragma once

#include "Engine/Math/Matrix4.hpp"

#include <string>

struct TagComponent {
    std::string Tag{};

    TagComponent() noexcept = default;
    TagComponent(const TagComponent& other) noexcept = default;
    TagComponent(TagComponent&& r_other) noexcept = default;
    TagComponent(const std::string& tag) noexcept : Tag{tag} {}

    operator const std::string&() const noexcept { return Tag; }
    operator std::string&() noexcept { return Tag; }
};

struct TransformComponent {
    Matrix4 Transform{};

    TransformComponent() noexcept = default;
    TransformComponent(const TransformComponent& other) noexcept = default;
    TransformComponent(TransformComponent&& r_other) noexcept = default;
    TransformComponent(const Matrix4& transform) noexcept
    : Transform{transform} {
    }

    operator const Matrix4&() const noexcept { return Transform; }
    operator Matrix4&() noexcept { return Transform; }

};
