#pragma once

#include <array>
#include <string>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Quaternion.hpp"

class AABB3;
class Camera3D;

class Matrix4 {
public:
    static const Matrix4 I;
    [[deprecated("Use Matrix::I instead.")]] static Matrix4 GetIdentity() noexcept;

    static Matrix4 CreateTranslationMatrix(const Vector2& position) noexcept;
    static Matrix4 CreateTranslationMatrix(const Vector3& position) noexcept;

    static Matrix4 Create2DRotationDegreesMatrix(float angleDegrees) noexcept;
    static Matrix4 Create3DXRotationDegreesMatrix(float angleDegrees) noexcept;
    static Matrix4 Create3DYRotationDegreesMatrix(float angleDegrees) noexcept;
    static Matrix4 Create3DZRotationDegreesMatrix(float angleDegrees) noexcept;

    static Matrix4 Create2DRotationMatrix(float angleRadians) noexcept;
    static Matrix4 Create3DXRotationMatrix(float angleRadians) noexcept;
    static Matrix4 Create3DYRotationMatrix(float angleRadians) noexcept;
    static Matrix4 Create3DZRotationMatrix(float angleRadians) noexcept;
    static Matrix4 CreateScaleMatrix(float scale) noexcept;
    static Matrix4 CreateScaleMatrix(const Vector2& scale) noexcept;
    static Matrix4 CreateScaleMatrix(const Vector3& scale) noexcept;
    static Matrix4 CreateTransposeMatrix(const Matrix4& mat) noexcept;
    static Matrix4 CreatePerspectiveProjectionMatrix(float top, float bottom, float right, float left, float nearZ, float farZ) noexcept;
    static Matrix4 CreateHPerspectiveProjectionMatrix(float fov, float aspect_ratio, float nearZ, float farZ) noexcept;
    static Matrix4 CreateVPerspectiveProjectionMatrix(float fov, float aspect_ratio, float nearZ, float farZ) noexcept;
    static Matrix4 CreateDXOrthographicProjection(float nx, float fx, float ny, float fy, float nz, float fz) noexcept;
    static Matrix4 CreateDXOrthographicProjection(const AABB3& extents) noexcept;
    static Matrix4 CreateDXPerspectiveProjection(float vfovDegrees, float aspect, float nz, float fz) noexcept;
    static Matrix4 CreateOrthographicProjectionMatrix(float top, float bottom, float right, float left, float nearZ, float farZ) noexcept;
    static Matrix4 CreateLookAtMatrix(const Vector3& cameraPosition, const Vector3& lookAt, const Vector3& worldUp) noexcept;
    static Matrix4 CalculateChangeOfBasisMatrix(const Matrix4& output_basis, const Matrix4& input_basis = Matrix4::GetIdentity()) noexcept;

    static Matrix4 MakeSRT(const Matrix4& S, const Matrix4& R, const Matrix4& T) noexcept;
    static Matrix4 MakeRT(const Matrix4& R, const Matrix4& T) noexcept;
    static Matrix4 MakeViewProjection(const Matrix4& viewMatrix, const Matrix4& projectionMatrix) noexcept;

    Matrix4() = default;
    explicit Matrix4(const std::string& value) noexcept;
    Matrix4(const Matrix4& other) = default;
    Matrix4(Matrix4&& other) = default;
    Matrix4& operator=(Matrix4&& rhs) = default;
    Matrix4& operator=(const Matrix4& rhs) = default;
    ~Matrix4() = default;

    explicit Matrix4(const Quaternion& q) noexcept;
    explicit Matrix4(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation = Vector2::ZERO) noexcept;
    explicit Matrix4(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation = Vector3::ZERO) noexcept;
    explicit Matrix4(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation = Vector4::ZERO_XYZ_ONE_W) noexcept;
    explicit Matrix4(const float* arrayOfFloats) noexcept;

    void Identity() noexcept;
    void Transpose() noexcept;
    float CalculateTrace() const noexcept;
    float CalculateTrace() noexcept;
    Vector4 GetDiagonal() const noexcept;
    static Vector4 GetDiagonal(const Matrix4& mat) noexcept;

    bool IsInvertable() const noexcept;
    bool IsSingular() const noexcept;

    void CalculateInverse() noexcept;
    static float CalculateDeterminant(const Matrix4& mat) noexcept;
    float CalculateDeterminant() const noexcept;
    float CalculateDeterminant() noexcept;
    static Matrix4 CalculateInverse(const Matrix4& mat) noexcept;

    void OrthoNormalizeIKJ() noexcept;
    void OrthoNormalizeIJK() noexcept;

    void Translate(const Vector2& translation2D) noexcept;
    void Translate(const Vector3& translation3D) noexcept;

    void Scale(float scale) noexcept;
    void Scale(const Vector2& scale) noexcept;
    void Scale(const Vector3& scale) noexcept;
    void Scale(const Vector4& scale) noexcept;

    void Rotate3DXDegrees(float degrees) noexcept;
    void Rotate3DYDegrees(float degrees) noexcept;
    void Rotate3DZDegrees(float degrees) noexcept;
    void Rotate2DDegrees(float degrees) noexcept;

    void Rotate3DXRadians(float radians) noexcept;
    void Rotate3DYRadians(float radians) noexcept;
    void Rotate3DZRadians(float radians) noexcept;
    void Rotate2DRadians(float radians) noexcept;

    void ConcatenateTransform(const Matrix4& other) noexcept;
    Matrix4 GetTransformed(const Matrix4& other) const noexcept;

    Vector2 TransformPosition(const Vector2& position) const noexcept;
    Vector2 TransformDirection(const Vector2& direction) const noexcept;

    Vector3 TransformPosition(const Vector3& position) const noexcept;
    Vector3 TransformDirection(const Vector3& direction) const noexcept;

    Vector4 TransformVector(const Vector4& homogeneousVector) const noexcept;
    Vector3 TransformVector(const Vector3& homogeneousVector) const noexcept;
    Vector2 TransformVector(const Vector2& homogeneousVector) const noexcept;

    const float* GetAsFloatArray() const noexcept;
    float* GetAsFloatArray() noexcept;

    Vector3 GetTranslation() const noexcept;
    Vector3 GetTranslation() noexcept;

    Vector3 GetScale() const noexcept;
    Vector3 GetScale() noexcept;

    Matrix4 GetRotation() const noexcept;
    Matrix4 GetRotation() noexcept;

    Vector3 CalcEulerAngles() const noexcept;

    //SRTs must be calculated as T * R * S
    Matrix4 operator*(const Matrix4& rhs) const noexcept;
    Vector4 operator*(const Vector4& rhs) const noexcept;
    friend Vector4 operator*(const Vector4& lhs, const Matrix4& rhs) noexcept;
    Vector3 operator*(const Vector3& rhs) const noexcept;
    friend Vector3 operator*(const Vector3& lhs, const Matrix4& rhs) noexcept;
    Vector2 operator*(const Vector2& rhs) const noexcept;
    friend Vector2 operator*(const Vector2& lhs, const Matrix4& rhs) noexcept;
    Matrix4& operator*=(const Matrix4& rhs) noexcept;
    friend Matrix4 operator*(float lhs, const Matrix4& rhs) noexcept;
    const float * operator*() const noexcept;
    float* operator*() noexcept;

    bool operator==(const Matrix4& rhs) const noexcept;
    bool operator==(const Matrix4& rhs) noexcept;
    bool operator!=(const Matrix4& rhs) const noexcept;
    bool operator!=(const Matrix4& rhs) noexcept;
    Matrix4 operator*(float scalar) const noexcept;
    Matrix4& operator*=(float scalar) noexcept;
    Matrix4 operator+(const Matrix4& rhs) const noexcept;
    Matrix4& operator+=(const Matrix4& rhs) noexcept;
    Matrix4 operator-(const Matrix4& rhs) const noexcept;
    Matrix4& operator-=(const Matrix4& rhs) noexcept;
    Matrix4 operator-() const noexcept;
    Matrix4 operator/(const Matrix4& rhs) noexcept;
    Matrix4& operator/=(const Matrix4& rhs) noexcept;

    friend std::ostream& operator<<(std::ostream& out_stream, const Matrix4& m) noexcept;
    friend std::istream& operator>>(std::istream& in_stream, Matrix4& m) noexcept;

    Vector4 GetIBasis() const noexcept;
    Vector4 GetIBasis() noexcept;

    Vector4 GetJBasis() const noexcept;
    Vector4 GetJBasis() noexcept;

    Vector4 GetKBasis() const noexcept;
    Vector4 GetKBasis() noexcept;

    Vector4 GetTBasis() const noexcept;
    Vector4 GetTBasis() noexcept;

    Vector4 GetXComponents() const noexcept;
    Vector4 GetXComponents() noexcept;

    Vector4 GetYComponents() const noexcept;
    Vector4 GetYComponents() noexcept;

    Vector4 GetZComponents() const noexcept;
    Vector4 GetZComponents() noexcept;

    Vector4 GetWComponents() const noexcept;
    Vector4 GetWComponents() noexcept;

    void SetIBasis(const Vector4& basis) noexcept;
    void SetJBasis(const Vector4& basis) noexcept;
    void SetKBasis(const Vector4& basis) noexcept;
    void SetTBasis(const Vector4& basis) noexcept;

    void SetXComponents(const Vector4& components) noexcept;
    void SetYComponents(const Vector4& components) noexcept;
    void SetZComponents(const Vector4& components) noexcept;
    void SetWComponents(const Vector4& components) noexcept;


protected:

    const float& operator[](std::size_t index) const;
    float& operator[](std::size_t index);

    void SetIndex(unsigned int index, float value) noexcept;
    float GetIndex(unsigned int index) const noexcept;
    float GetIndex(unsigned int index) noexcept;
    float GetIndex(unsigned int col, unsigned int row) const noexcept;

    static Matrix4 CreateTranslationMatrix(float x, float y, float z) noexcept;
    static Matrix4 CreateScaleMatrix(float scale_x, float scale_y, float scale_z) noexcept;

    explicit Matrix4(float m00, float m01, float m02, float m03,
                     float m10, float m11, float m12, float m13,
                     float m20, float m21, float m22, float m23,
                     float m30, float m31, float m32, float m33) noexcept;


private:
    //[00 01 02 03] [0   1  2  3] 
    //[10 11 12 13] [4   5  6  7]
    //[20 21 22 23] [8   9 10 11]
    //[30 31 32 33] [12 13 14 15]

    std::array<float, 16> m_indicies{ 1.0f, 0.0f, 0.0f, 0.0f,
                                      0.0f, 1.0f, 0.0f, 0.0f,
                                      0.0f, 0.0f, 1.0f, 0.0f,
                                      0.0f, 0.0f, 0.0f, 1.0f };

    friend class Quaternion;

};
