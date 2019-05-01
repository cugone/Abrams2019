#pragma once

#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Vector3.hpp"

#include <array>

class Matrix4;
class Camera3D;

class Frustum {
public:
    static Frustum CreateFromViewProjectionMatrix(const Matrix4& viewProjection, float aspectRatio, float vfovDegrees, const Vector3& forward, float near, float far, bool normalize);
    static Frustum CreateFromCamera(const Camera3D& camera, bool normalize);

    Frustum(const Frustum& other) = default;
    Frustum(Frustum&& other) = default;
    Frustum& operator=(const Frustum& rhs) = default;
    Frustum& operator=(Frustum&& rhs) = default;
    ~Frustum() = default;

    const Plane3& GetLeft() const;
    const Plane3& GetRight() const;
    const Plane3& GetTop() const;
    const Plane3& GetBottom() const;
    const Plane3& GetNear() const;
    const Plane3& GetFar() const;

    const Vector3& GetNearBottomLeft() const;
    const Vector3& GetNearTopLeft() const;
    const Vector3& GetNearTopRight() const;
    const Vector3& GetNearBottomRight() const;
    const Vector3& GetFarBottomLeft() const;
    const Vector3& GetFarTopLeft() const;
    const Vector3& GetFarTopRight() const;
    const Vector3& GetFarBottomRight() const;

protected:
private:
    explicit Frustum(const Matrix4& viewProjection, float aspectRatio, float vfovDegrees, const Vector3& forward, float near, float far, bool normalize);

    void SetLeft(const Plane3& left);
    void SetRight(const Plane3& right);
    void SetTop(const Plane3& top);
    void SetBottom(const Plane3& bottom);
    void SetNear(const Plane3& near);
    void SetFar(const Plane3& far);

    void CalcPoints(float aspectRatio, float vfovDegrees, const Vector3& forward, float near, float far);

    enum class PlaneDirection {
        Left,
        Right,
        Top,
        Bottom,
        Near,
        Far,
        Max
    };
    std::array<Plane3, static_cast<std::size_t>(PlaneDirection::Max)> _planes{};
    std::array<Vector3, 8> _points{};

};