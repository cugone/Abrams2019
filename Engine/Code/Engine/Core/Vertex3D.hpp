#pragma once

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

class InputLayout;

class Vertex3D {
public:

    Vertex3D(const Vector3& pos = Vector3::ZERO, const Rgba& color = Rgba::White, const Vector2& tex_coords = Vector2::ZERO, const Vector3& normal = Vector3::Z_AXIS);
    Vertex3D(const Vertex3D& other) = default;
    Vertex3D(Vertex3D&& other) = default;
    Vertex3D& operator=(const Vertex3D& other) = default;
    Vertex3D& operator=(Vertex3D&& other) = default;
    Vector3 position = Vector3::ZERO;
    Vector4 color = Rgba::White.GetRgbaAsFloats();
    Vector2 texcoords = Vector2::ZERO;
    Vector3 normal = Vector3::Z_AXIS;

protected:
private:
};

inline Vertex3D::Vertex3D(const Vector3& pos /*= Vector3::ZERO*/
                          , const Rgba& color /*= Rgba::WHITE*/
                          , const Vector2& tex_coords /*= Vector2::ZERO*/
                          , const Vector3& normal /*= Vector3::Z_AXIS*/)
    : position(pos)
    , color(color.GetRgbaAsFloats())
    , texcoords(tex_coords)
    , normal(normal)
{
    /* DO NOTHING */
}