#pragma once

#include "Engine/ECS/ECS.hpp"

class Scene {
public:
    Scene() = default;
    Scene(const Scene& other) = default;
    Scene(Scene&& other) = default;
    Scene& operator=(const Scene& other) = default;
    Scene& operator=(Scene&& other) = default;
    ~Scene() = default;
protected:
private:
    
};
