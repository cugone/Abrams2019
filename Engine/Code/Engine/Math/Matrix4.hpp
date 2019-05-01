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

    static Matrix4 GetIdentity();
    static Matrix4 CreateTranslationMatrix(const Vector2& position);
    static Matrix4 CreateTranslationMatrix(const Vector3& position);

    static Matrix4 Create2DRotationDegreesMatrix(float angleDegrees);
    static Matrix4 Create3DXRotationDegreesMatrix(float angleDegrees);
    static Matrix4 Create3DYRotationDegreesMatrix(float angleDegrees);
    static Matrix4 Create3DZRotationDegreesMatrix(float angleDegrees);

    static Matrix4 Create2DRotationMatrix(float angleRadians);
    static Matrix4 Create3DXRotationMatrix(float angleRadians);
    static Matrix4 Create3DYRotationMatrix(float angleRadians);
    static Matrix4 Create3DZRotationMatrix(float angleRadians);
    static Matrix4 CreateScaleMatrix(float scale);
    static Matrix4 CreateScaleMatrix(const Vector2& scale);
    static Matrix4 CreateScaleMatrix(const Vector3& scale);
    static Matrix4 CreateTransposeMatrix(const Matrix4& mat);
    static Matrix4 CreatePerspectiveProjectionMatrix(float top, float bottom, float right, float left, float nearZ, float farZ);
    static Matrix4 CreateHPerspectiveProjectionMatrix(float fov, float aspect_ratio, float nearZ, float farZ);
    static Matrix4 CreateVPerspectiveProjectionMatrix(float fov, float aspect_ratio, float nearZ, float farZ);
    static Matrix4 CreateDXOrthographicProjection(float nx, float fx, float ny, float fy, float nz, float fz);
    static Matrix4 CreateDXOrthographicProjection(const AABB3& extents);
    static Matrix4 CreateDXPerspectiveProjection(float vfovDegrees, float aspect, float nz, float fz);
    static Matrix4 CreateOrthographicProjectionMatrix(float top, float bottom, float right, float left, float nearZ, float farZ);
    static Matrix4 CreateLookAtMatrix(const Vector3& cameraPosition, const Vector3& lookAt, const Vector3& worldUp);
    static Matrix4 CalculateChangeOfBasisMatrix(const Matrix4& output_basis, const Matrix4& input_basis = Matrix4::GetIdentity());

    Matrix4() = default;
    explicit Matrix4(const std::string& value);
    Matrix4(const Matrix4& other) = default;
    Matrix4(Matrix4&& other) = default;
    Matrix4& operator=(Matrix4&& rhs) = default;
    Matrix4& operator=(const Matrix4& rhs) = default;
    ~Matrix4() = default;

    explicit Matrix4(const Quaternion& q);
    explicit Matrix4(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation = Vector2::ZERO);
    explicit Matrix4(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation = Vector3::ZERO);
    explicit Matrix4(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation = Vector4::ZERO_XYZ_ONE_W);
    explicit Matrix4(const float* arrayOfFloats);

    void Identity();
    void Transpose();
    float CalculateTrace() const;
    float CalculateTrace();
    Vector4 GetDiagonal() const;
    static Vector4 GetDiagonal(const Matrix4& mat);

    bool IsInvertable() const;
    bool IsSingular() const;

    void CalculateInverse();
    static float CalculateDeterminant(const Matrix4& mat);
    float CalculateDeterminant() const;
    float CalculateDeterminant();
    static Matrix4 CalculateInverse(const Matrix4& mat);

    void OrthoNormalizeIKJ();

    void Translate(const Vector2& translation2D);
    void Translate(const Vector3& translation3D);

    void Scale(float scale);
    void Scale(const Vector2& scale);
    void Scale(const Vector3& scale);
    void Scale(const Vector4& scale);

    void Rotate3DXDegrees(float degrees);
    void Rotate3DYDegrees(float degrees);
    void Rotate3DZDegrees(float degrees);
    void Rotate2DDegrees(float degrees);

    void Rotate3DXRadians(float radians);
    void Rotate3DYRadians(float radians);
    void Rotate3DZRadians(float radians);
    void Rotate2DRadians(float radians);

    void ConcatenateTransform(const Matrix4& other);
    Matrix4 GetTransformed(const Matrix4& other) const;

    Vector2 TransformPosition(const Vector2& position) const;
    Vector2 TransformDirection(const Vector2& direction) const;

    Vector3 TransformPosition(const Vector3& position) const;
    Vector3 TransformDirection(const Vector3& direction) const;

    Vector4 TransformVector(const Vector4& homogeneousVector) const;
    Vector3 TransformVector(const Vector3& homogeneousVector) const;
    Vector2 TransformVector(const Vector2& homogeneousVector) const;

    const float* GetAsFloatArray() const;
    float* GetAsFloatArray();

    Vector3 GetTranslation() const;
    Vector3 GetTranslation();

    Vector3 GetScale() const;
    Vector3 GetScale();

    Matrix4 GetRotation() const;
    Matrix4 GetRotation();

    Vector3 CalcEulerAngles() const;

    Matrix4 operator*(const Matrix4& rhs) const;
    Vector4 operator*(const Vector4& rhs) const;
    Vector3 operator*(const Vector3& rhs) const;
    Vector2 operator*(const Vector2& rhs) const;
    Matrix4& operator*=(const Matrix4& rhs);
    friend Matrix4 operator*(float lhs, const Matrix4& rhs);
    const float * operator*() const;
    float* operator*();

    bool operator==(const Matrix4& rhs) const;
    bool operator==(const Matrix4& rhs);
    bool operator!=(const Matrix4& rhs) const;
    bool operator!=(const Matrix4& rhs);
    Matrix4 operator*(float scalar) const;
    Matrix4& operator*=(float scalar);
    Matrix4 operator+(const Matrix4& rhs) const;
    Matrix4& operator+=(const Matrix4& rhs);
    Matrix4 operator-(const Matrix4& rhs) const;
    Matrix4& operator-=(const Matrix4& rhs);
    Matrix4 operator-() const;
    Matrix4 operator/(const Matrix4& rhs);
    Matrix4& operator/=(const Matrix4& rhs);

    friend std::ostream& operator<<(std::ostream& out_stream, const Matrix4& m);
    friend std::istream& operator>>(std::istream& in_stream, Matrix4& m);

    Vector4 GetIBasis() const;
    Vector4 GetIBasis();

    Vector4 GetJBasis() const;
    Vector4 GetJBasis();

    Vector4 GetKBasis() const;
    Vector4 GetKBasis();

    Vector4 GetTBasis() const;
    Vector4 GetTBasis();

    Vector4 GetXComponents() const;
    Vector4 GetXComponents();

    Vector4 GetYComponents() const;
    Vector4 GetYComponents();

    Vector4 GetZComponents() const;
    Vector4 GetZComponents();

    Vector4 GetWComponents() const;
    Vector4 GetWComponents();

    void SetIBasis(const Vector4& basis);
    void SetJBasis(const Vector4& basis);
    void SetKBasis(const Vector4& basis);
    void SetTBasis(const Vector4& basis);

    void SetXComponents(const Vector4& components);
    void SetYComponents(const Vector4& components);
    void SetZComponents(const Vector4& components);
    void SetWComponents(const Vector4& components);


protected:

    const float& operator[](std::size_t index) const;
    float& operator[](std::size_t index);

    void SetIndex(unsigned int index, float value);
    float GetIndex(unsigned int index) const;
    float GetIndex(unsigned int index);
    float GetIndex(unsigned int col, unsigned int row) const;

    static Matrix4 CreateTranslationMatrix(float x, float y, float z);
    static Matrix4 CreateScaleMatrix(float scale_x, float scale_y, float scale_z);

    explicit Matrix4(float m00, float m01, float m02, float m03,
                     float m10, float m11, float m12, float m13,
                     float m20, float m21, float m22, float m23,
                     float m30, float m31, float m32, float m33);


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
