#pragma once

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Scene/ECS.hpp"
#include "Engine/Scene/Scene.hpp"

#include <memory>

class Entity {
public:
    Entity() noexcept = default;
    Entity(const Entity& other) noexcept = default;
    Entity(Entity&& other) noexcept = default;
    Entity& operator=(const Entity& rhs) noexcept = default;
    Entity& operator=(Entity&& rhs) noexcept = default;
    ~Entity() noexcept = default;

    Entity(std::uint32_t handle, std::weak_ptr<Scene> scene) noexcept;

    [[nodiscard]] operator bool() const noexcept {
        return m_id != entt::null;
    }

    [[nodiscard]] bool HasParent() const noexcept;
    [[nodiscard]] Entity* GetParent() const noexcept;

    [[nodiscard]] bool HasChildren() const noexcept;
    [[nodiscard]] const std::vector<Entity>& GetChildren() const noexcept;
    [[nodiscard]] std::vector<Entity>& GetChildren() noexcept;


    [[nodiscard]] bool HasComponents() const noexcept;

    template<typename... Component>
    [[nodiscard]] bool HasAllOfComponents() const noexcept {
        GUARANTEE_OR_DIE(!m_Scene.expired(), "Entity scene context has expired!");
        return m_Scene.lock()->m_registry.all_of<Component...>();
    }
    
    template<typename... Component>
    [[nodiscard]] bool HasAnyOfComponents() const noexcept {
        GUARANTEE_OR_DIE(!m_Scene.expired(), "Entity scene context has expired!");
        return m_Scene.lock()->m_registry.any_of<Component...>();
    }

    template<typename Component>
    [[nodiscard]] bool HasComponent() const noexcept {
        return HasAllOfComponents<Component>();
    }

    template<typename Component, typename... Args>
    Component& AddComponent(Args&&... args) noexcept {
        GUARANTEE_OR_DIE(!m_Scene.expired(), "Entity scene context has expired!");
        GUARANTEE_OR_DIE(!HasComponent<Component>(), "Entity already has specified component!");
        return m_Scene.lock()->m_registry.emplace<Component>(m_id, std::forward<Args>(Args)...);
    }

    template<typename Component>
    [[nodiscard]] const Component& GetComponent() const noexcept {
        GUARANTEE_OR_DIE(!m_Scene.expired(), "Entity scene context has expired!");
        GUARANTEE_OR_DIE(HasComponent<Component>(), "Entity does not have specified component!");
        return m_Scene.lock()->m_registry.get<Component>(m_id);
    }
    
    template<typename Component>
    [[nodiscard]] Component& GetComponent() noexcept {
        GUARANTEE_OR_DIE(!m_Scene.expired(), "Entity scene context has expired!");
        GUARANTEE_OR_DIE(HasComponent<Component>(), "Entity does not have specified component!");
        m_Scene.lock()->m_registry.get<Component>(m_id);
    }
    
    template<typename Component>
    void RemoveComponent() noexcept {
        GUARANTEE_OR_DIE(!m_Scene.expired(), "Entity scene context has expired!");
        GUARANTEE_OR_DIE(HasComponent<Component>(), "Entity does not have specified component!");
        m_Scene.lock()->m_registry.remove<Component>(m_id);
    }

protected:
private:
    entt::entity m_id{entt::null};
    std::weak_ptr<Scene> m_Scene{};
    Entity* m_parent{nullptr};
    std::vector<Entity> m_children{};

    friend class Scene;
};
