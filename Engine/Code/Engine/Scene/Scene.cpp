#include "Engine/Scene/Scene.hpp"

#include "Engine/Scene/Components.hpp"
#include "Engine/Scene/Entity.hpp"

#include <memory>

Entity Scene::CreateEntity() noexcept {
    Entity entity = {static_cast<std::uint32_t>(m_registry.create()), weak_from_this() };
    entity.AddComponent<IdComponent>();
}

Entity Scene::CreateEntityWithUUID(UUID uuid) noexcept {
    Entity entity = {static_cast<std::uint32_t>(m_registry.create()), weak_from_this()};
    entity.AddComponent<IdComponent>(uuid);
}

void Scene::DestroyEntity(Entity e) noexcept {
    m_registry.destroy(e.m_id);
}

std::weak_ptr<const Scene> Scene::get() const noexcept {
    return weak_from_this();
}

std::weak_ptr<Scene> Scene::get() noexcept {
    return weak_from_this();
}

const entt::registry& Scene::GetRegistry() const noexcept {
    return m_registry;
}

entt::registry& Scene::GetRegistry() noexcept {
    return m_registry;
}
