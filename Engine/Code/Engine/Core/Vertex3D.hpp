#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

class InputLayout;

struct Vertex3D {
    // clang-format off
    Vertex3D(const Vector3& pos = Vector3::ZERO
            ,const Rgba& color = Rgba::White
            ,const Vector2& tex_coords = Vector2::ZERO
            ,const Vector3& normal = Vector3::Z_AXIS
            ,const Vector3& tangent = Vector3::ZERO
            ,const Vector3& bitangent = Vector3::ZERO) noexcept
    : position(pos)
    , texcoords(tex_coords)
    , normal(normal)
    , tangent(tangent)
    , bitangent(bitangent) {
        const auto [r, g, b, a] = color.GetAsFloats();
        this->color.x = r;
        this->color.y = g;
        this->color.z = b;
        this->color.w = a;
    }
    // clang-format on
    Vertex3D(const Vertex3D& other) = default;
    Vertex3D(Vertex3D&& other) = default;
    Vertex3D& operator=(const Vertex3D& other) = default;
    Vertex3D& operator=(Vertex3D&& other) = default;
    Vector3 position = Vector3::ZERO;
    Vector4 color = Vector4::ONE;
    Vector2 texcoords = Vector2::ZERO;
    Vector3 normal = Vector3::Z_AXIS;
    Vector3 tangent = Vector3::ZERO;
    Vector3 bitangent = Vector3::ZERO;

protected:
private:
};
