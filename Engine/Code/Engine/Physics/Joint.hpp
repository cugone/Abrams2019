#pragma once

class RigidBody;

class Joint {
public:
    Joint() = default;
    Joint(const Joint& other) = default;
    Joint(Joint&& other) = default;
    Joint& operator=(const Joint& other) = default;
    Joint& operator=(Joint&& other) = default;
    virtual ~Joint() = default;

protected:
    RigidBody* a{};
    RigidBody* b{};
private:
    
};
