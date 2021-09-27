#pragma once

#include "Engine/Math/Matrix4.hpp"
#include "Engine/Renderer/Mesh.hpp"

#include <string>

struct TagComponent {
    std::string Tag{};

    TagComponent() noexcept = default;
    TagComponent(const TagComponent& other) noexcept = default;
    TagComponent(TagComponent&& r_other) noexcept = default;
    TagComponent& operator=(const TagComponent& rhs) noexcept = default;
    TagComponent& operator=(TagComponent&& rhs) noexcept = default;
    ~TagComponent() noexcept = default;
    explicit TagComponent(const std::string& tag) noexcept : Tag{tag} {}
    operator const std::string&() const noexcept { return Tag; }
    operator std::string&() noexcept { return Tag; }
};

struct TransformComponent {
    Matrix4 Transform{};

    TransformComponent() noexcept = default;
    TransformComponent(const TransformComponent& other) noexcept = default;
    TransformComponent(TransformComponent&& r_other) noexcept = default;
    TransformComponent& operator=(const TransformComponent& rhs) noexcept = default;
    TransformComponent& operator=(TransformComponent&& rhs) noexcept = default;
    explicit TransformComponent(const Matrix4& transform) noexcept
    : Transform{transform} {
    }
    ~TransformComponent() noexcept = default;
    operator const Matrix4&() const noexcept { return Transform; }
    operator Matrix4&() noexcept { return Transform; }

};

struct MeshComponent {
    Mesh mesh{};

    MeshComponent() noexcept = default;
    MeshComponent(const MeshComponent& other) noexcept = default;
    MeshComponent(MeshComponent&& r_other) noexcept = default;
    MeshComponent& operator=(const MeshComponent& rhs) noexcept = default;
    MeshComponent& operator=(MeshComponent&& rhs) noexcept = default;
    ~MeshComponent() noexcept = default;
    explicit MeshComponent(const Mesh& newMesh) noexcept
    : mesh{newMesh} {}

    operator const Mesh&() const noexcept {
        return mesh;
    }
    operator Mesh&() noexcept {
        return mesh;
    }

};

struct SceneComponent {
    TransformComponent Transform{};

    SceneComponent() noexcept = default;
    SceneComponent(const SceneComponent& other) noexcept = default;
    SceneComponent(SceneComponent&& r_other) noexcept = default;
    SceneComponent& operator=(const SceneComponent& rhs) noexcept = default;
    SceneComponent& operator=(SceneComponent&& rhs) noexcept = default;
    ~SceneComponent() noexcept = default;
    explicit SceneComponent(const TransformComponent& transform) noexcept
    : Transform{transform} {}

};

struct SpriteSheetComponent {
    AnimatedSpriteDesc AnimatedSprite{};

    SpriteSheetComponent() noexcept = default;
    SpriteSheetComponent(const SpriteSheetComponent& other) noexcept = default;
    SpriteSheetComponent(SpriteSheetComponent&& r_other) noexcept = default;
    SpriteSheetComponent& operator=(const SpriteSheetComponent& rhs) noexcept = default;
    SpriteSheetComponent& operator=(SpriteSheetComponent&& rhs) noexcept = default;
    ~SpriteSheetComponent() noexcept = default;
    SpriteSheetComponent(const AnimatedSpriteDesc& desc) noexcept
    : AnimatedSprite{desc} {}

};

struct RenderComponent {
    std::string MaterialName{"_2D"};
    Rgba Tint{};

    RenderComponent() noexcept = default;
    RenderComponent(const RenderComponent& other) noexcept = default;
    RenderComponent(RenderComponent&& r_other) noexcept = default;
    RenderComponent& operator=(const RenderComponent& rhs) noexcept = default;
    RenderComponent& operator=(RenderComponent&& rhs) noexcept = default;
    ~RenderComponent() noexcept = default;
    RenderComponent(const std::string& materialName, const Rgba& tint = Rgba::White) noexcept
    : MaterialName{materialName}
    , Tint{tint} {}

};
