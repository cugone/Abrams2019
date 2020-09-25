#pragma once

#include "Engine/Math/Vector3.hpp"

#include <vector>

class RigidBody;

struct PhysicsMaterial {
    float friction = 0.0f;    //0.7f; //Range: [0.0,1.0]; How quickly an object comes to rest during a contact. Values closer to 1.0 cause resting contacts to lose velocity faster.
    float restitution = 0.0f; //0.3f; //Range: [-1.0f, 1.0f]; The bouncyness of a material. Negative values cause an object to gain velocity after a collision.
    float density = 1.0f; //Affect mass calculation for "bigger" objects.
    float massExponent = 1.0f; // 0.75f; //Raise final mass calculation to this exponent.
};

struct PhysicsDesc {
    float mass = 1.0f; //How "heavy" an object is. Expressed in Kilograms. Cannot be lower than 0.001f;
    float maxAngularSpeed = 1000.0f;
    float linearDamping = 0.90f;
    float angularDamping = 0.90f;
    bool enableGravity = true; //Should gravity be applied.
    bool enableDrag = true;   //Should drag be applied.
    bool enablePhysics = true; //Should object be subject to physics calculations.
    bool startAwake = true;    //Should the object be awake on creation.
};

struct GJKResult {
    bool collides{false};
    std::vector<Vector3> simplex{};
};

struct EPAResult {
    float distance{0.0f};
    Vector3 normal{};
};

struct CollisionData {
    RigidBody* const a = nullptr;
    RigidBody* const b = nullptr;
    float distance = 0.0f;
    Vector3 normal{};
    CollisionData(RigidBody* const a, RigidBody* const b, float distance, const Vector3& normal);
    bool operator==(const CollisionData& rhs) const noexcept;
    bool operator!=(const CollisionData& rhs) const noexcept;
};
