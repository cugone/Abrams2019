#pragma once

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
