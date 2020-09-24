#include "Engine/Physics/Joint.hpp"


void Joint::attach(RigidBody* a, RigidBody* b) noexcept {
    bodyA = a;
    bodyB = b;
}

void Joint::detach(RigidBody* body) noexcept {
    if(bodyA == body) {
        bodyA = nullptr;
    }
    if(bodyB == body) {
        bodyB = nullptr;
    }
}

void Joint::detach_all() noexcept {
    bodyA = nullptr;
    bodyB = nullptr;
}

bool Joint::is_not_attached() const noexcept {
    return bodyA == nullptr && bodyB == nullptr;
}
