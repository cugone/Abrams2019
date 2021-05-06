#include "Engine/Renderer/Camera.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

namespace a2de {

    void Camera::SetProjectionMode(ProjectionMode newProjectionMode) noexcept {
        projection_mode = newProjectionMode;
    }

    ProjectionMode Camera::GetProjectionMode() const noexcept {
        return projection_mode;
    }

    void Camera::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
        trauma -= trauma_recovery_rate * deltaSeconds.count();
        if(trauma < 0.0f) {
            trauma = 0.0f;
        }
    }

    const Vector3& Camera::GetPosition() const noexcept {
        return position;
    }

    void Camera::SetPosition(const Vector3& newPosition) noexcept {
        position = newPosition;
    }

    void Camera::SetPosition(const Vector2& newPosition) noexcept {
        position = Vector3{newPosition, 0.0f};
    }

    void Camera::Translate(const Vector3& displacement) noexcept {
        position += displacement;
    }

    void Camera::Translate(const Vector2& displacement) noexcept {
        position += Vector3{displacement, 0.0f};
    }

    void Camera::SetOffsets(const Matrix4& transform, float fov) noexcept {
        positionOffset = transform.GetTranslation();
        const auto eulers = transform.CalcEulerAngles();
        rotationPitchOffset = eulers.x;
        rotationYawOffset = eulers.y;
        rotationRollOffset = eulers.z;
        fovOffset = fov;
    }

    void Camera::AddOffsets(const Matrix4& transform, float fov) noexcept {
        positionOffset += transform.GetTranslation();
        const auto eulers = transform.CalcEulerAngles();
        rotationPitchOffset += eulers.x;
        rotationYawOffset += eulers.y;
        rotationRollOffset += eulers.z;
        fovOffset += fov;
    }

    void Camera::ClearOffsets() noexcept {
        positionOffset = Vector3{};
        rotationPitchOffset = 0.0f;
        rotationYawOffset = 0.0f;
        rotationRollOffset = 0.0f;
        fovOffset = 0.0f;
    }

    float Camera::GetAspectRatio() const noexcept {
        return aspect_ratio;
    }

    float Camera::GetInverseAspectRatio() const noexcept {
        return 1.0f / aspect_ratio;
    }

    float Camera::GetNearDistance() const noexcept {
        return near_distance;
    }

    float Camera::GetFarDistance() const noexcept {
        return far_distance;
    }

    const Matrix4& Camera::GetViewMatrix() const noexcept {
        return view_matrix;
    }

    const Matrix4& Camera::GetProjectionMatrix() const noexcept {
        return projection_matrix;
    }

    const Matrix4& Camera::GetViewProjectionMatrix() const noexcept {
        return view_projection_matrix;
    }

    const Matrix4& Camera::GetInverseViewMatrix() const noexcept {
        return inv_view_matrix;
    }

    const Matrix4& Camera::GetInverseProjectionMatrix() const noexcept {
        return inv_projection_matrix;
    }

    const Matrix4& Camera::GetInverseViewProjectionMatrix() const noexcept {
        return inv_view_projection_matrix;
    }

    const RenderTargetStack::Node& Camera::GetRenderTarget() const noexcept {
        return _render_target;
    }

    RenderTargetStack::Node& Camera::GetRenderTarget() noexcept {
        return const_cast<RenderTargetStack::Node&>(static_cast<const Camera&>(*this).GetRenderTarget());
    }

    float Camera::GetShake() const noexcept {
        return trauma * trauma;
    }

    const Matrix4& Camera::GetRotationMatrix() const noexcept {
        return rotation_matrix;
    }

    Matrix4 Camera::CreateBillboardMatrix(const Matrix4& rotationMatrix) noexcept {
        return Matrix4::MakeSRT(rotationMatrix, Matrix4::Create3DYRotationDegreesMatrix(180.0f), inv_view_matrix.GetRotation());
    }

    Matrix4 Camera::CreateReverseBillboardMatrix(const Matrix4& rotationMatrix) noexcept {
        return Matrix4::MakeRT(rotationMatrix, inv_view_matrix.GetRotation());
    }

    Vector3 Camera::GetEulerAngles() const noexcept {
        return Vector3{GetPitch(), GetYaw(), GetRoll()};
    }

    Vector3 Camera::GetEulerAnglesDegrees() const noexcept {
        return Vector3{GetPitchDegrees(), GetYawDegrees(), GetRollDegrees()};
    }

    void Camera::SetEulerAnglesDegrees(const Vector3& eulerAnglesDegrees) noexcept {
        SetEulerAngles(Vector3{MathUtils::ConvertDegreesToRadians(eulerAnglesDegrees.x),
                        MathUtils::ConvertDegreesToRadians(eulerAnglesDegrees.y),
                        MathUtils::ConvertDegreesToRadians(eulerAnglesDegrees.z)});
    }

    void Camera::SetEulerAngles(const Vector3& eulerAngles) noexcept {
        rotationPitch = eulerAngles.x;
        rotationYaw = eulerAngles.y;
        rotationRoll = eulerAngles.z;
    }

    void Camera::SetForwardFromTarget(const Vector3& lookAtPosition) noexcept {
        switch(projection_mode) {
        case ProjectionMode::Perspective:
        {
            Vector3 forward = (lookAtPosition - position).GetNormalize();
            Vector3 right = MathUtils::CrossProduct(world_up.GetNormalize(), forward);
            Vector3 up = MathUtils::CrossProduct(forward, right);
            Matrix4 m;
            m.SetIBasis(Vector4(right, 0.0f));
            m.SetJBasis(Vector4(up, 0.0f));
            m.SetKBasis(Vector4(forward, 0.0f));
            rotation = Quaternion(m);
            auto eulerangles = rotation.CalcEulerAnglesDegrees();
            rotationPitch = eulerangles.x;
            rotationYaw = eulerangles.y;
            rotationRoll = eulerangles.z;
            break;
        }
        case ProjectionMode::Orthographic:
        {
            /* DO NOTHING */
            break;
        }
        }
    }

    Vector3 Camera::GetRight() const noexcept {
        switch(projection_mode) {
        case ProjectionMode::Perspective:
        {
            const auto forward = GetForward();
            return MathUtils::CrossProduct(world_up, forward);
        }
        case ProjectionMode::Orthographic:
        {
            Vector2 up = -Vector2::Y_AXIS;
            up.SetHeadingRadians(rotationRoll);
            return Vector3{up.GetRightHandNormal(), 0.0f};
        }
        default:
            ERROR_AND_DIE("Camera::GetRight: ProjectionMode enum has changed");
        }
    }

    Vector3 Camera::GetUp() const noexcept {
        switch(projection_mode) {
        case ProjectionMode::Perspective:
        {
            const auto forward = GetForward();
            const auto right = GetRight();
            return MathUtils::CrossProduct(forward, right);
        }
        case ProjectionMode::Orthographic:
        {
            Vector2 up = -Vector2::Y_AXIS;
            up.SetHeadingRadians(rotationRoll);
            return Vector3{up, 0.0f};
        }
        default:
            ERROR_AND_DIE("Camera::GetUp: ProjectionMode enum has changed");
        }
    }

    Vector3 Camera::GetForward() const noexcept {
        switch(projection_mode) {
        case ProjectionMode::Perspective:
        {
            const auto cos_yaw = MathUtils::CosDegrees(rotationYaw);
            const auto cos_pitch = MathUtils::CosDegrees(rotationPitch);
            const auto sin_yaw = MathUtils::SinDegrees(rotationYaw);
            const auto sin_pitch = MathUtils::SinDegrees(rotationPitch);
            return Vector3(-sin_yaw * cos_pitch, sin_pitch, cos_yaw * cos_pitch);
        }
        case ProjectionMode::Orthographic:
        {
            return Vector3::Z_AXIS;
        }
        default:
            ERROR_AND_DIE("Camera::GetForward: ProjectionMode enum has changed");
        }
    }

    float Camera::GetYawDegrees() const noexcept {
        return MathUtils::ConvertRadiansToDegrees(GetYaw());
    }

    float Camera::GetPitchDegrees() const noexcept {
        return MathUtils::ConvertRadiansToDegrees(GetPitch());
    }

    float Camera::GetRollDegrees() const noexcept {
        return MathUtils::ConvertRadiansToDegrees(GetRoll());
    }

    float Camera::GetYaw() const noexcept {
        return rotationYaw;
    }

    float Camera::GetPitch() const noexcept {
        return rotationPitch;
    }

    float Camera::GetRoll() const noexcept {
        return rotationRoll;
    }

} // namespace a2de
