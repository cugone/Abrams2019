#pragma once

#include "Engine/Scene/ECS.hpp"

#include <memory>

class Entity;

class Scene : std::enable_shared_from_this<Scene> {
public:
    Scene() = default;
    Scene(const Scene& other) = default;
    Scene(Scene&& other) = default;
    Scene& operator=(const Scene& other) = default;
    Scene& operator=(Scene&& other) = default;
    ~Scene() = default;

    Entity CreateEntity() noexcept;
    void DestroyEntity(Entity e) noexcept;

    std::weak_ptr<const Scene> get() const noexcept;
    std::weak_ptr<Scene> get() noexcept;

protected:
private:
    entt::registry m_registry{};
    
    friend class Entity;

};
