#include "Engine/Physics/ForceGenerator.hpp"

#include "Engine/Physics/RigidBody.hpp"

#include <algorithm>

ForceGenerator::~ForceGenerator() {/* DO NOTHING */}

void ForceGenerator::attach(RigidBody* body) noexcept {
    _observers.push_back(body);
}

void ForceGenerator::detach(RigidBody* body) noexcept {
    _observers.erase(std::remove_if(std::begin(_observers), std::end(_observers), [body](const RigidBody* a) { return a == body; }), std::end(_observers));
}
