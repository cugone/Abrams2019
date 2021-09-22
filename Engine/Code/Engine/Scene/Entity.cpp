#include "Engine/Scene/Entity.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Scene/Scene.hpp"

Entity::Entity(std::uint32_t handle, std::weak_ptr<Scene> scene) noexcept
    : m_id(static_cast<entt::entity>(handle))
{
    GUARANTEE_OR_DIE(!scene.expired(), "Scene reference has expired.");
    m_Scene = scene;
}

bool Entity::HasParent() const noexcept {
    return m_parent != nullptr;
}

Entity* Entity::GetParent() const noexcept {
    return m_parent;
}

bool Entity::HasChildren() const noexcept {
    return m_children.empty();
}

const std::vector<Entity*>& Entity::GetChildren() const noexcept {
    return m_children;
}

std::vector<Entity*>& Entity::GetChildren() noexcept {
    return m_children;
}

bool Entity::HasComponents() const noexcept {
    GUARANTEE_OR_DIE(!m_Scene.expired(), "Scene reference has expired.");
    return m_Scene.lock()->m_registry.orphan(m_id) == false;
}
