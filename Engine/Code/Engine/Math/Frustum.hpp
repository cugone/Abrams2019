#pragma once

#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Vector3.hpp"

#include <array>

class Matrix4;
class Camera3D;

class Frustum {
public:
    static Frustum CreateFromViewProjectionMatrix(const Matrix4& viewProjection, float aspectRatio, float vfovDegrees, const Vector3& forward, float near, float far, bool normalize) noexcept;
    static Frustum CreateFromCamera(const Camera3D& camera, bool normalize) noexcept;

    Frustum(const Frustum& other) = default;
    Frustum(Frustum&& other) = default;
    Frustum& operator=(const Frustum& rhs) = default;
    Frustum& operator=(Frustum&& rhs) = default;
    ~Frustum() = default;

    const Plane3& GetLeft() const noexcept;
    const Plane3& GetRight() const noexcept;
    const Plane3& GetTop() const noexcept;
    const Plane3& GetBottom() const noexcept;
    const Plane3& GetNear() const noexcept;
    const Plane3& GetFar() const noexcept;

    const Vector3& GetNearBottomLeft() const noexcept;
    const Vector3& GetNearTopLeft() const noexcept;
    const Vector3& GetNearTopRight() const noexcept;
    const Vector3& GetNearBottomRight() const noexcept;
    const Vector3& GetFarBottomLeft() const noexcept;
    const Vector3& GetFarTopLeft() const noexcept;
    const Vector3& GetFarTopRight() const noexcept;
    const Vector3& GetFarBottomRight() const noexcept;

protected:
private:
    explicit Frustum(const Matrix4& viewProjection, float aspectRatio, float vfovDegrees, const Vector3& forward, float near, float far, bool normalize) noexcept;

    void SetLeft(const Plane3& left) noexcept;
    void SetRight(const Plane3& right) noexcept;
    void SetTop(const Plane3& top) noexcept;
    void SetBottom(const Plane3& bottom) noexcept;
    void SetNear(const Plane3& near) noexcept;
    void SetFar(const Plane3& far) noexcept;

    void CalcPoints(float aspectRatio, float vfovDegrees, const Vector3& forward, float near, float far) noexcept;

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