#include "Engine/Physics/Joint.hpp"


void Joint::Attach(RigidBody* a, RigidBody* b) noexcept {
    bodyA = a;
    bodyB = b;
}

void Joint::Detach(RigidBody* body) noexcept {
    if(bodyA == body) {
        bodyA = nullptr;
    }
    if(bodyB == body) {
        bodyB = nullptr;
    }
}

void Joint::DetachAll() noexcept {
    bodyA = nullptr;
    bodyB = nullptr;
}

bool Joint::IsNotAttached() const noexcept {
    return bodyA == nullptr && bodyB == nullptr;
}

void Joint::AttachedCanCollide(bool canCollide) noexcept {
    attachedCanCollide = canCollide;
}

RigidBody* Joint::GetBodyA() const noexcept {
    return bodyA;
}

RigidBody* Joint::GetBodyB() const noexcept {
    return bodyB;
}
