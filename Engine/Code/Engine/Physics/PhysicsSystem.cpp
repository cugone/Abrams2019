#include "Engine/Physics/PhysicsSystem.hpp"


void PhysicsSystem::Update_Worker(TimeUtils::FPSeconds /*deltaSeconds*/) noexcept {

}

PhysicsSystem::PhysicsSystem(const PhysicsSystemDesc& desc /*= PhysicsSystemDesc{}*/)
	: desc(desc)
{
	/* DO NOTHING */
}

PhysicsSystem::~PhysicsSystem() {
	/* DO NOTHING */
}

void PhysicsSystem::Initialize() noexcept {
	/* DO NOTHING */
}

void PhysicsSystem::BeginFrame() noexcept {
	/* DO NOTHING */
}

void PhysicsSystem::Update(TimeUtils::FPSeconds /*deltaSeconds*/) noexcept {
	/* DO NOTHING */
}

void PhysicsSystem::Render() const noexcept {
	/* DO NOTHING */
}

void PhysicsSystem::EndFrame() noexcept {
	/* DO NOTHING */
}
