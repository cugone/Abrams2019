#include "Engine/Math/Matrix4.hpp"

#include <sstream>

#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"

const Matrix4 Matrix4::I{};

Matrix4::Matrix4(const std::string& value) noexcept {
    if(value[0] == '[') {
        if(value.back() == ']') {
            std::stringstream ss(value.substr(1, value.size() - 1));
            std::string curLine;
            for(int i = 0; std::getline(ss, curLine, ','); ++i) {
                m_indicies[i] = std::stof(curLine);
            }
        }
    }
}

Matrix4::Matrix4(float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33) noexcept {
    m_indicies[0] = m00; m_indicies[1] = m01; m_indicies[2] = m02; m_indicies[3] = m03;
    m_indicies[4] = m10; m_indicies[5] = m11; m_indicies[6] = m12; m_indicies[7] = m13;
    m_indicies[8] = m20; m_indicies[9] = m21; m_indicies[10] = m22; m_indicies[11] = m23;
    m_indicies[12] = m30; m_indicies[13] = m31; m_indicies[14] = m32; m_indicies[15] = m33;
}

Matrix4::Matrix4(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation /*= Vector4::ZERO_XYZ_ONE_W*/) noexcept {
    m_indicies[0] = iBasis.x; m_indicies[1] = jBasis.x; m_indicies[2] = kBasis.x; m_indicies[3] = translation.x;
    m_indicies[4] = iBasis.y; m_indicies[5] = jBasis.y; m_indicies[6] = kBasis.y; m_indicies[7] = translation.y;
    m_indicies[8] = iBasis.z; m_indicies[9] = jBasis.z; m_indicies[10] = kBasis.z; m_indicies[11] = translation.z;
    m_indicies[12] = iBasis.w; m_indicies[13] = jBasis.w; m_indicies[14] = kBasis.w; m_indicies[15] = translation.w;
}

Matrix4::Matrix4(const float* arrayOfFloats) noexcept {
    m_indicies[0] = arrayOfFloats[0];   m_indicies[1] = arrayOfFloats[1];   m_indicies[2] = arrayOfFloats[2];   m_indicies[3] = arrayOfFloats[3];
    m_indicies[4] = arrayOfFloats[4];   m_indicies[5] = arrayOfFloats[5];   m_indicies[6] = arrayOfFloats[6];   m_indicies[7] = arrayOfFloats[7];
    m_indicies[8] = arrayOfFloats[8];   m_indicies[9] = arrayOfFloats[9];   m_indicies[10] = arrayOfFloats[10];  m_indicies[11] = arrayOfFloats[11];
    m_indicies[12] = arrayOfFloats[12];  m_indicies[13] = arrayOfFloats[13];  m_indicies[14] = arrayOfFloats[14];  m_indicies[15] = arrayOfFloats[15];
}

Matrix4::Matrix4(const Quaternion& q) noexcept {

    auto q_norm = q.GetNormalize();

    float x = q.axis.x;
    float y = q.axis.y;
    float z = q.axis.z;
    float w = q.w;

    Matrix4 left;
    left.m_indicies[0] = w; left.m_indicies[1] = -z; left.m_indicies[2] = y; left.m_indicies[3] = -x;
    left.m_indicies[4] = z; left.m_indicies[5] = w; left.m_indicies[6] = -x; left.m_indicies[7] = -y;
    left.m_indicies[8] = -y; left.m_indicies[9] = x; left.m_indicies[10] = w; left.m_indicies[11] = -z;
    left.m_indicies[12] = x; left.m_indicies[13] = y; left.m_indicies[14] = z; left.m_indicies[15] = w;

    Matrix4 right;
    right.m_indicies[0] = w; right.m_indicies[1] = -z; right.m_indicies[2] = y; right.m_indicies[3] = x;
    right.m_indicies[4] = z; right.m_indicies[5] = w; right.m_indicies[6] = -x; right.m_indicies[7] = y;
    right.m_indicies[8] = -y; right.m_indicies[9] = x; right.m_indicies[10] = w; right.m_indicies[11] = z;
    right.m_indicies[12] = -x; right.m_indicies[13] = -y; right.m_indicies[14] = -z; right.m_indicies[15] = w;

    m_indicies = (left * right).m_indicies;

}
Matrix4::Matrix4(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation /*= Vector2::ZERO*/) noexcept
    : m_indicies{ iBasis.x, jBasis.x, 0.0f, translation.x,
    iBasis.y, jBasis.y, 0.0f, translation.y,
    0.0f,     0.0f, 1.0f,          0.0f,
    0.0f,     0.0f, 0.0f,          1.0f } {
    /* DO NOTHING */
}
Matrix4::Matrix4(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation /*= Vector3::ZERO*/) noexcept
    : m_indicies{ iBasis.x, jBasis.x, kBasis.x, translation.x,
    iBasis.y, jBasis.y, kBasis.y, translation.y,
    iBasis.z, jBasis.z, kBasis.z, translation.z,
    0.0f,     0.0f,     0.0f,          1.0f } {
    /* DO NOTHING */
}

Matrix4 Matrix4::GetIdentity() noexcept {
    return Matrix4(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateTranslationMatrix(float x, float y, float z) noexcept {
    return Matrix4(1.0f, 0.0f, 0.0f, x,
        0.0f, 1.0f, 0.0f, y,
        0.0f, 0.0f, 1.0f, z,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateTranslationMatrix(const Vector3& position) noexcept {
    return CreateTranslationMatrix(position.x, position.y, position.z);
}

Matrix4 Matrix4::CreateTranslationMatrix(const Vector2& position) noexcept {
    return CreateTranslationMatrix(position.x, position.y, 0.0f);
}

Matrix4 Matrix4::Create2DRotationDegreesMatrix(float angleDegrees) noexcept {
    return Create2DRotationMatrix(MathUtils::ConvertDegreesToRadians(angleDegrees));
}
Matrix4 Matrix4::Create3DXRotationDegreesMatrix(float angleDegrees) noexcept {
    return Create3DXRotationMatrix(MathUtils::ConvertDegreesToRadians(angleDegrees));
}
Matrix4 Matrix4::Create3DYRotationDegreesMatrix(float angleDegrees) noexcept {
    return Create3DYRotationMatrix(MathUtils::ConvertDegreesToRadians(angleDegrees));
}
Matrix4 Matrix4::Create3DZRotationDegreesMatrix(float angleDegrees) noexcept {
    return Create3DZRotationMatrix(MathUtils::ConvertDegreesToRadians(angleDegrees));
}

Matrix4 Matrix4::Create2DRotationMatrix(float angleRadians) noexcept {
    return Create3DZRotationMatrix(angleRadians);
}

Matrix4 Matrix4::Create3DXRotationMatrix(float angle) noexcept {
    return Matrix4(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, std::cos(angle), -std::sin(angle), 0.0f,
        0.0f, std::sin(angle), std::cos(angle), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::Create3DYRotationMatrix(float angle) noexcept {
    return Matrix4(std::cos(angle), 0.0f, std::sin(angle), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -std::sin(angle), 0.0f, std::cos(angle), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::Create3DZRotationMatrix(float angle) noexcept {
    return Matrix4(std::cos(angle), -std::sin(angle), 0.0f, 0.0f,
        std::sin(angle), std::cos(angle), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateScaleMatrix(float scale_x, float scale_y, float scale_z) noexcept {
    return Matrix4(scale_x, 0.0f, 0.0f, 0.0f,
        0.0f, scale_y, 0.0f, 0.0f,
        0.0f, 0.0f, scale_z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateScaleMatrix(const Vector3& scale) noexcept {
    return CreateScaleMatrix(scale.x, scale.y, scale.z);
}

Matrix4 Matrix4::CreateScaleMatrix(const Vector2& scale) noexcept {
    return CreateScaleMatrix(Vector3(scale, 1.0f));
}
Matrix4 Matrix4::CreateScaleMatrix(float scale) noexcept {
    return CreateScaleMatrix(Vector3(scale, scale, scale));
}

Matrix4 Matrix4::CalculateChangeOfBasisMatrix(const Matrix4& output_basis, const Matrix4& input_basis /*= Matrix4::GetIdentity()*/) noexcept {
    return Matrix4::CalculateInverse(output_basis) * input_basis;
}

void Matrix4::SetIBasis(const Vector4& iBasis) noexcept {
    m_indicies[0] = iBasis.x;
    m_indicies[4] = iBasis.y;
    m_indicies[8] = iBasis.z;
    m_indicies[12] = iBasis.w;
}

void Matrix4::SetJBasis(const Vector4& jBasis) noexcept {
    m_indicies[1] = jBasis.x;
    m_indicies[5] = jBasis.y;
    m_indicies[9] = jBasis.z;
    m_indicies[13] = jBasis.w;
}

void Matrix4::SetKBasis(const Vector4& kBasis) noexcept {
    m_indicies[2] = kBasis.x;
    m_indicies[6] = kBasis.y;
    m_indicies[10] = kBasis.z;
    m_indicies[14] = kBasis.w;
}

void Matrix4::SetTBasis(const Vector4& tBasis) noexcept {
    m_indicies[3] = tBasis.x;
    m_indicies[7] = tBasis.y;
    m_indicies[11] = tBasis.z;
    m_indicies[15] = tBasis.w;
}

void Matrix4::SetXComponents(const Vector4& components) noexcept {
    m_indicies[0] = components.x;
    m_indicies[1] = components.y;
    m_indicies[2] = components.z;
    m_indicies[3] = components.w;
}

void Matrix4::SetYComponents(const Vector4& components) noexcept {
    m_indicies[4] = components.x;
    m_indicies[5] = components.y;
    m_indicies[6] = components.z;
    m_indicies[7] = components.w;
}

void Matrix4::SetZComponents(const Vector4& components) noexcept {
    m_indicies[8] = components.x;
    m_indicies[9] = components.y;
    m_indicies[10] = components.z;
    m_indicies[11] = components.w;
}

void Matrix4::SetWComponents(const Vector4& components) noexcept {
    m_indicies[12] = components.x;
    m_indicies[13] = components.y;
    m_indicies[14] = components.z;
    m_indicies[15] = components.w;
}

Vector4 Matrix4::GetIBasis() const noexcept {
    return Vector4(m_indicies[0], m_indicies[4], m_indicies[8], m_indicies[12]);
}

Vector4 Matrix4::GetIBasis() noexcept {
    return static_cast<const Matrix4&>(*this).GetIBasis();
}

Vector4 Matrix4::GetJBasis() const noexcept {
    return Vector4(m_indicies[1], m_indicies[5], m_indicies[9], m_indicies[13]);
}

Vector4 Matrix4::GetJBasis() noexcept {
    return static_cast<const Matrix4&>(*this).GetJBasis();
}

Vector4 Matrix4::GetKBasis() const noexcept {
    return Vector4(m_indicies[2], m_indicies[6], m_indicies[10], m_indicies[14]);
}

Vector4 Matrix4::GetKBasis() noexcept {
    return static_cast<const Matrix4&>(*this).GetKBasis();
}

Vector4 Matrix4::GetTBasis() const noexcept {
    return Vector4(m_indicies[3], m_indicies[7], m_indicies[11], m_indicies[15]);
}

Vector4 Matrix4::GetTBasis() noexcept {
    return static_cast<const Matrix4&>(*this).GetTBasis();
}

Vector4 Matrix4::GetXComponents() const noexcept {
    return Vector4(m_indicies[0], m_indicies[1], m_indicies[2], m_indicies[3]);
}

Vector4 Matrix4::GetXComponents() noexcept {
    return static_cast<const Matrix4&>(*this).GetXComponents();
}

Vector4 Matrix4::GetYComponents() const noexcept {
    return Vector4(m_indicies[4], m_indicies[5], m_indicies[6], m_indicies[7]);
}

Vector4 Matrix4::GetYComponents() noexcept {
    return static_cast<const Matrix4&>(*this).GetYComponents();
}

Vector4 Matrix4::GetZComponents() const noexcept {
    return Vector4(m_indicies[8], m_indicies[9], m_indicies[10], m_indicies[11]);
}

Vector4 Matrix4::GetZComponents() noexcept {
    return static_cast<const Matrix4&>(*this).GetZComponents();
}

Vector4 Matrix4::GetWComponents() const noexcept {
    return Vector4(m_indicies[12], m_indicies[13], m_indicies[14], m_indicies[15]);
}

Vector4 Matrix4::GetWComponents() noexcept {
    return static_cast<const Matrix4&>(*this).GetWComponents();
}

void Matrix4::SetIndex(unsigned int index, float value) noexcept {
    m_indicies[index] = value;
}

float Matrix4::GetIndex(unsigned int index) const noexcept {
    return m_indicies[index];
}

float Matrix4::GetIndex(unsigned int index) noexcept {
    return static_cast<const Matrix4&>(*this).GetIndex(index);
}

float Matrix4::GetIndex(unsigned int col, unsigned int row) const noexcept {
    return GetIndex(4 * col + row);
}

void Matrix4::Identity() noexcept {

    m_indicies[0] = 1.0f;  m_indicies[1] = 0.0f;  m_indicies[2] = 0.0f;  m_indicies[3] = 0.0f;
    m_indicies[4] = 0.0f;  m_indicies[5] = 1.0f;  m_indicies[6] = 0.0f;  m_indicies[7] = 0.0f;
    m_indicies[8] = 0.0f;  m_indicies[9] = 0.0f;  m_indicies[10] = 1.0f;  m_indicies[11] = 0.0f;
    m_indicies[12] = 0.0f;  m_indicies[13] = 0.0f;  m_indicies[14] = 0.0f;  m_indicies[15] = 1.0f;

}
void Matrix4::Transpose() noexcept {

    //[00 01 02 03] [0   1  2  3]
    //[10 11 12 13] [4   5  6  7]
    //[20 21 22 23] [8   9 10 11]
    //[30 31 32 33] [12 13 14 15]
    //
    //[00 10 20 30] [0 4 8  12]
    //[01 11 21 31] [1 5 9  13]
    //[02 12 22 32] [2 6 10 14]
    //[03 13 23 33] [3 7 11 15]

    std::swap(m_indicies[1], m_indicies[4]);
    std::swap(m_indicies[2], m_indicies[8]);
    std::swap(m_indicies[3], m_indicies[12]);

    std::swap(m_indicies[6], m_indicies[9]);
    std::swap(m_indicies[7], m_indicies[13]);
    std::swap(m_indicies[11], m_indicies[14]);

}

Matrix4 Matrix4::CreateTransposeMatrix(const Matrix4& mat) noexcept {
    return Matrix4(mat.m_indicies[0], mat.m_indicies[4], mat.m_indicies[8], mat.m_indicies[12],
        mat.m_indicies[1], mat.m_indicies[5], mat.m_indicies[9], mat.m_indicies[13],
        mat.m_indicies[2], mat.m_indicies[6], mat.m_indicies[10], mat.m_indicies[14],
        mat.m_indicies[3], mat.m_indicies[7], mat.m_indicies[11], mat.m_indicies[15]);
}

Matrix4 Matrix4::CreatePerspectiveProjectionMatrix(float top, float bottom, float right, float left, float nearZ, float farZ) noexcept {
    return Matrix4(((2.0f * nearZ) / (right - left)), 0.0f, ((right + left) / (right - left)), 0.0f
        , 0.0f, 2.0f / (top - bottom), ((top + bottom) / (top - bottom)), 0.0f
        , 0.0f, 0.0f, ((-2.0f) / (farZ - nearZ)), (-(farZ + nearZ) / (farZ - nearZ))
        , 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateHPerspectiveProjectionMatrix(float fov, float /*aspect_ratio*/, float nearZ, float farZ) noexcept {
    float S = 1.0f / std::tan(MathUtils::ConvertDegreesToRadians(fov / 2.0f));
    return Matrix4(S, 0.0f, 0.0f, 0.0f,
        0.0f, S, 0.0f, 0.0f,
        0.0f, 0.0f, -(farZ / (farZ - nearZ)), -1.0f,
        0.0f, 0.0f, -((farZ * nearZ) / (farZ - nearZ)), 0.0f);
}

Matrix4 Matrix4::CreateVPerspectiveProjectionMatrix(float fov, float aspect_ratio, float nearZ, float farZ) noexcept {
    float f = std::atan(MathUtils::ConvertDegreesToRadians(fov) / 2.0f);
    return Matrix4(f / aspect_ratio, 0.0f, 0.0f, 0.0f,
        0.0f, f, 0.0f, 0.0f,
        0.0f, 0.0f, ((farZ + nearZ) / (nearZ - farZ)), (2.0f * farZ * nearZ) / (nearZ - farZ),
        0.0f, 0.0f, -1.0f, 0.0f);
}

Matrix4 Matrix4::CreateDXOrthographicProjection(float nx, float fx, float ny, float fy, float nz, float fz) noexcept {

    float sx = 2.0f / (fx - nx);
    float sy = 2.0f / (fy - ny);
    float sz = 1.0f / (fz - nz);
    float tx = -(fx + nx) * sx;
    float ty = -(fy + ny) * sy;
    float tz = -nz * sz;
    Matrix4 mat(sx, 0.0f, 0.0f, tx,
        0.0f, sy, 0.0f, ty,
        0.0f, 0.0f, sz, tz,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    return mat;
}

Matrix4 Matrix4::CreateDXOrthographicProjection(const AABB3& extents) noexcept {
    return CreateDXOrthographicProjection(extents.mins.x, extents.maxs.x, extents.mins.y, extents.maxs.y, extents.mins.z, extents.maxs.z);
}

Matrix4 Matrix4::CreateDXPerspectiveProjection(float vfovDegrees, float aspect, float nz, float fz) noexcept {
    float fov_rads = MathUtils::ConvertDegreesToRadians(vfovDegrees);
    float inv_tan = 1.0f / std::tan(fov_rads * 0.50f);
    float inv_aspect = 1.0f / aspect;
    float depth = fz - nz;
    float nzfz = nz * fz;
    float inv_depth = 1.0f / depth;

    float sx = inv_tan * inv_aspect;
    float sy = inv_tan;
    float sz = fz * inv_depth;
    float tz = -nzfz * inv_depth;
    Matrix4 mat(sx, 0.0f, 0.0f, 0.0f
        , 0.0f, sy, 0.0f, 0.0f
        , 0.0f, 0.0f, sz, tz
        , 0.0f, 0.0f, 1.0f, 0.0f
    );
    //mat.Transpose();
    return mat;
}
Matrix4 Matrix4::CreateOrthographicProjectionMatrix(float top, float bottom, float right, float left, float nearZ, float farZ) noexcept {
    return Matrix4(2.0f / (right - left), 0.0f, 0.0f, -((right + left) / (right - left))
        , 0.0f, 2.0f / (top - bottom), 0.0f, -((top + bottom) / (top - bottom))
        , 0.0f, 0.0f, (-2.0f / (farZ - nearZ)), (-(farZ + nearZ) / (farZ - nearZ))
        , 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateLookAtMatrix(const Vector3& eye, const Vector3& lookAt, const Vector3& up) noexcept {
    Vector3 cam_forward = (lookAt - eye).GetNormalize();
    Vector3 world_up = up.GetNormalize();
    Vector3 cam_right = MathUtils::CrossProduct(world_up, cam_forward).GetNormalize();
    Vector3 cam_up = MathUtils::CrossProduct(cam_forward, cam_right);

    Matrix4 R(cam_right.x, cam_up.x, cam_forward.x, 0.0f,
        cam_right.y, cam_up.y, cam_forward.y, 0.0f,
        cam_right.z, cam_up.z, cam_forward.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    //R.Transpose();

    Matrix4 T(1.0f, 0.0f, 0.0f, -eye.x,
        0.0f, 1.0f, 0.0f, -eye.y,
        0.0f, 0.0f, 1.0f, -eye.z,
        0.0f, 0.0f, 0.0f, 1.0f);
    //T.Transpose();

    Matrix4 L = T * R;

    return L;
}

void Matrix4::CalculateInverse() noexcept {
    *this = Matrix4::CalculateInverse(*this);
}

Matrix4 Matrix4::CalculateInverse(const Matrix4& mat) noexcept {

    //Minors, Cofactors, Adjugates method.
    //See http://www.mathsisfun.com/algebra/matrix-inverse-minors-cofactors-adjugate.html

    //[00 01 02 03] [0   1  2  3]
    //[10 11 12 13] [4   5  6  7]
    //[20 21 22 23] [8   9 10 11]
    //[30 31 32 33] [12 13 14 15]

    //Calculate minors
    float m00 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[5], mat.m_indicies[6], mat.m_indicies[7], mat.m_indicies[9], mat.m_indicies[10], mat.m_indicies[11], mat.m_indicies[13], mat.m_indicies[14], mat.m_indicies[15]);
    float m01 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[4], mat.m_indicies[6], mat.m_indicies[7], mat.m_indicies[8], mat.m_indicies[10], mat.m_indicies[11], mat.m_indicies[12], mat.m_indicies[14], mat.m_indicies[15]);
    float m02 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[4], mat.m_indicies[5], mat.m_indicies[7], mat.m_indicies[8], mat.m_indicies[9], mat.m_indicies[11], mat.m_indicies[12], mat.m_indicies[13], mat.m_indicies[15]);
    float m03 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[4], mat.m_indicies[5], mat.m_indicies[6], mat.m_indicies[8], mat.m_indicies[9], mat.m_indicies[10], mat.m_indicies[12], mat.m_indicies[13], mat.m_indicies[14]);

    float m10 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[1], mat.m_indicies[2], mat.m_indicies[3], mat.m_indicies[9], mat.m_indicies[10], mat.m_indicies[11], mat.m_indicies[13], mat.m_indicies[14], mat.m_indicies[15]);
    float m11 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[0], mat.m_indicies[2], mat.m_indicies[3], mat.m_indicies[8], mat.m_indicies[10], mat.m_indicies[11], mat.m_indicies[12], mat.m_indicies[14], mat.m_indicies[15]);
    float m12 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[0], mat.m_indicies[1], mat.m_indicies[3], mat.m_indicies[8], mat.m_indicies[9], mat.m_indicies[11], mat.m_indicies[12], mat.m_indicies[13], mat.m_indicies[15]);
    float m13 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[0], mat.m_indicies[1], mat.m_indicies[2], mat.m_indicies[8], mat.m_indicies[9], mat.m_indicies[10], mat.m_indicies[12], mat.m_indicies[13], mat.m_indicies[14]);

    float m20 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[1], mat.m_indicies[2], mat.m_indicies[3], mat.m_indicies[5], mat.m_indicies[6], mat.m_indicies[7], mat.m_indicies[13], mat.m_indicies[14], mat.m_indicies[15]);
    float m21 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[0], mat.m_indicies[2], mat.m_indicies[3], mat.m_indicies[4], mat.m_indicies[6], mat.m_indicies[7], mat.m_indicies[12], mat.m_indicies[14], mat.m_indicies[15]);
    float m22 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[0], mat.m_indicies[1], mat.m_indicies[3], mat.m_indicies[4], mat.m_indicies[5], mat.m_indicies[7], mat.m_indicies[12], mat.m_indicies[13], mat.m_indicies[15]);
    float m23 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[0], mat.m_indicies[1], mat.m_indicies[2], mat.m_indicies[4], mat.m_indicies[5], mat.m_indicies[6], mat.m_indicies[12], mat.m_indicies[13], mat.m_indicies[14]);

    float m30 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[1], mat.m_indicies[2], mat.m_indicies[3], mat.m_indicies[5], mat.m_indicies[6], mat.m_indicies[7], mat.m_indicies[9], mat.m_indicies[10], mat.m_indicies[11]);
    float m31 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[0], mat.m_indicies[2], mat.m_indicies[3], mat.m_indicies[4], mat.m_indicies[6], mat.m_indicies[7], mat.m_indicies[8], mat.m_indicies[10], mat.m_indicies[11]);
    float m32 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[0], mat.m_indicies[1], mat.m_indicies[3], mat.m_indicies[4], mat.m_indicies[5], mat.m_indicies[7], mat.m_indicies[8], mat.m_indicies[9], mat.m_indicies[11]);
    float m33 = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[0], mat.m_indicies[1], mat.m_indicies[2], mat.m_indicies[4], mat.m_indicies[5], mat.m_indicies[6], mat.m_indicies[8], mat.m_indicies[9], mat.m_indicies[10]);

    Matrix4 cofactors(m00, -m01, m02, -m03,
                      -m10, m11, -m12, m13,
                      m20, -m21, m22, -m23,
                      -m30, m31, -m32, m33);

    Matrix4 adjugate(Matrix4::CreateTransposeMatrix(cofactors));

    float det_mat = mat.CalculateDeterminant();
    float inv_det = 1.0f / det_mat;

    return inv_det * adjugate;
}

void Matrix4::OrthoNormalizeIKJ() noexcept {
    Vector4 i = GetIBasis();
    Vector4 k = GetKBasis();
    Vector4 j = GetJBasis();

    i.Normalize3D();
    k -= MathUtils::Project(k, i);
    k.Normalize3D();
    j -= MathUtils::Project(j, i);
    j -= MathUtils::Project(i, k);
    j.Normalize3D();

    SetIBasis(i);
    SetJBasis(j);
    SetKBasis(k);

}

void Matrix4::OrthoNormalizeIJK() noexcept {
    Vector4 i = GetIBasis();
    Vector4 k = GetKBasis();
    Vector4 j = GetJBasis();

    i.Normalize3D();
    j -= MathUtils::Project(j, i);
    j.Normalize3D();
    k -= MathUtils::Project(k, i);
    k -= MathUtils::Project(i, j);
    k.Normalize3D();

    SetIBasis(i);
    SetJBasis(j);
    SetKBasis(k);

}

float Matrix4::CalculateDeterminant(const Matrix4& mat) noexcept {


    //[00 01 02 03] [0   1  2  3]
    //[10 11 12 13] [4   5  6  7]
    //[20 21 22 23] [8   9 10 11]
    //[30 31 32 33] [12 13 14 15]

    float a = mat.m_indicies[0];
    float det_not_a = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[5], mat.m_indicies[6], mat.m_indicies[7],
        mat.m_indicies[9], mat.m_indicies[10], mat.m_indicies[11],
        mat.m_indicies[13], mat.m_indicies[14], mat.m_indicies[15]);

    float b = mat.m_indicies[1];
    float det_not_b = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[4], mat.m_indicies[6], mat.m_indicies[7],
        mat.m_indicies[8], mat.m_indicies[10], mat.m_indicies[11],
        mat.m_indicies[12], mat.m_indicies[14], mat.m_indicies[15]);

    float c = mat.m_indicies[2];
    float det_not_c = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[4], mat.m_indicies[5], mat.m_indicies[7],
        mat.m_indicies[8], mat.m_indicies[9], mat.m_indicies[11],
        mat.m_indicies[12], mat.m_indicies[13], mat.m_indicies[15]);

    float d = mat.m_indicies[3];
    float det_not_d = MathUtils::CalculateMatrix3Determinant(mat.m_indicies[4], mat.m_indicies[5], mat.m_indicies[6],
        mat.m_indicies[8], mat.m_indicies[9], mat.m_indicies[10],
        mat.m_indicies[12], mat.m_indicies[13], mat.m_indicies[14]);

    return (a * det_not_a) - (b * det_not_b) + (c * det_not_c) - (d * det_not_d);
}

float Matrix4::CalculateDeterminant() const noexcept {
    return Matrix4::CalculateDeterminant(*this);
}

float Matrix4::CalculateDeterminant() noexcept {
    return static_cast<const Matrix4&>(*this).CalculateDeterminant();
}

float Matrix4::CalculateTrace() const noexcept {
    return (m_indicies[0] + m_indicies[5] + m_indicies[10] + m_indicies[15]);
}

float Matrix4::CalculateTrace() noexcept {
    return static_cast<const Matrix4&>(*this).CalculateTrace();
}

bool Matrix4::IsInvertable() const noexcept {
    return IsSingular() == false;
}
bool Matrix4::IsSingular() const noexcept {
    return MathUtils::IsEquivalent(CalculateDeterminant(), 0.0f);
}
void Matrix4::Translate(const Vector2& translation2D) noexcept {
    m_indicies[3] += translation2D.x;
    m_indicies[7] += translation2D.y;
}
void Matrix4::Translate(const Vector3& translation3D) noexcept {
    m_indicies[3] += translation3D.x;
    m_indicies[7] += translation3D.y;
    m_indicies[11] += translation3D.z;
}
void Matrix4::Scale(float scale) noexcept {
    Scale(Vector4(scale, scale, scale, scale));
}
void Matrix4::Scale(const Vector2& scale) noexcept {
    Scale(Vector4(scale, 1.0f, 1.0f));
}

void Matrix4::Scale(const Vector3& scale) noexcept {
    Scale(Vector4(scale, 1.0f));
}

void Matrix4::Scale(const Vector4& scale) noexcept {

    m_indicies[0] *= scale.x;
    m_indicies[1] *= scale.x;
    m_indicies[2] *= scale.x;

    m_indicies[4] *= scale.y;
    m_indicies[5] *= scale.y;
    m_indicies[6] *= scale.y;

    m_indicies[8] *= scale.z;
    m_indicies[9] *= scale.z;
    m_indicies[10] *= scale.z;

    m_indicies[12] *= scale.w;
    m_indicies[13] *= scale.w;
    m_indicies[14] *= scale.w;

}

void Matrix4::Rotate3DXDegrees(float degrees) noexcept {
    Rotate3DXRadians(MathUtils::ConvertDegreesToRadians(degrees));
}
void Matrix4::Rotate3DYDegrees(float degrees) noexcept {
    Rotate3DYRadians(MathUtils::ConvertDegreesToRadians(degrees));
}
void Matrix4::Rotate3DZDegrees(float degrees) noexcept {
    Rotate3DZRadians(MathUtils::ConvertDegreesToRadians(degrees));
}
void Matrix4::Rotate2DDegrees(float degrees) noexcept {
    Rotate3DZDegrees(degrees);
}
void Matrix4::Rotate3DXRadians(float radians) noexcept {

    float r = radians;
    float c = std::cos(r);
    float s = std::sin(r);

    m_indicies[5] = c;
    m_indicies[6] = -s;
    m_indicies[9] = s;
    m_indicies[10] = c;
}
void Matrix4::Rotate3DYRadians(float radians) noexcept {

    float r = radians;
    float c = std::cos(r);
    float s = std::sin(r);

    m_indicies[0] = c;
    m_indicies[2] = s;
    m_indicies[8] = -s;
    m_indicies[10] = c;

}
void Matrix4::Rotate3DZRadians(float radians) noexcept {

    float r = radians;
    float c = std::cos(r);
    float s = std::sin(r);

    m_indicies[0] = c;
    m_indicies[1] = -s;
    m_indicies[4] = s;
    m_indicies[5] = c;

}
void Matrix4::Rotate2DRadians(float radians) noexcept {
    Rotate3DZRadians(radians);
}
void Matrix4::ConcatenateTransform(const Matrix4& other) noexcept {
    operator*=(other);
}
Matrix4 Matrix4::GetTransformed(const Matrix4& other) const noexcept {
    return operator*(other);
}
Vector2 Matrix4::TransformPosition(const Vector2& position) const noexcept {
    Vector4 v(position.x, position.y, 0.0f, 1.0f);

    float x = MathUtils::DotProduct(GetXComponents(), v);
    float y = MathUtils::DotProduct(GetYComponents(), v);

    return Vector2(x, y);
}
Vector3 Matrix4::TransformPosition(const Vector3& position) const noexcept {
    Vector4 v(position.x, position.y, position.z, 1.0f);

    float x = MathUtils::DotProduct(GetXComponents(), v);
    float y = MathUtils::DotProduct(GetYComponents(), v);
    float z = MathUtils::DotProduct(GetZComponents(), v);

    return Vector3(x, y, z);
}
Vector2 Matrix4::TransformDirection(const Vector2& direction) const noexcept {
    Vector4 v(direction.x, direction.y, 0.0f, 0.0f);

    float x = MathUtils::DotProduct(GetXComponents(), v);
    float y = MathUtils::DotProduct(GetYComponents(), v);

    return Vector2(x, y);
}
Vector3 Matrix4::TransformDirection(const Vector3& direction) const noexcept {
    Vector4 v(direction.x, direction.y, direction.z, 0.0f);

    float x = MathUtils::DotProduct(GetXComponents(), v);
    float y = MathUtils::DotProduct(GetYComponents(), v);
    float z = MathUtils::DotProduct(GetZComponents(), v);

    return Vector3(x, y, z);
}
Vector4 Matrix4::TransformVector(const Vector4& homogeneousVector) const noexcept {
    return operator*(homogeneousVector);
}

Vector3 Matrix4::TransformVector(const Vector3& homogeneousVector) const noexcept {
    return operator*(homogeneousVector);
}

Vector2 Matrix4::TransformVector(const Vector2& homogeneousVector) const noexcept {
    return operator*(homogeneousVector);
}

Vector4 Matrix4::GetDiagonal() const noexcept {
    return Matrix4::GetDiagonal(*this);
}
Vector4 Matrix4::GetDiagonal(const Matrix4& mat) noexcept {
    return Vector4(mat.m_indicies[0], mat.m_indicies[5], mat.m_indicies[10], mat.m_indicies[15]);
}
bool Matrix4::operator==(const Matrix4& rhs) const noexcept {
    return (MathUtils::IsEquivalent(m_indicies[0], rhs.m_indicies[0]) && MathUtils::IsEquivalent(m_indicies[1], rhs.m_indicies[1]) && MathUtils::IsEquivalent(m_indicies[2], rhs.m_indicies[2]) && MathUtils::IsEquivalent(m_indicies[3], rhs.m_indicies[3]) &&
        MathUtils::IsEquivalent(m_indicies[4], rhs.m_indicies[4]) && MathUtils::IsEquivalent(m_indicies[5], rhs.m_indicies[5]) && MathUtils::IsEquivalent(m_indicies[6], rhs.m_indicies[6]) && MathUtils::IsEquivalent(m_indicies[7], rhs.m_indicies[7]) &&
        MathUtils::IsEquivalent(m_indicies[8], rhs.m_indicies[8]) && MathUtils::IsEquivalent(m_indicies[9], rhs.m_indicies[9]) && MathUtils::IsEquivalent(m_indicies[10], rhs.m_indicies[10]) && MathUtils::IsEquivalent(m_indicies[11], rhs.m_indicies[11]) &&
        MathUtils::IsEquivalent(m_indicies[12], rhs.m_indicies[12]) && MathUtils::IsEquivalent(m_indicies[13], rhs.m_indicies[13]) && MathUtils::IsEquivalent(m_indicies[14], rhs.m_indicies[14]) && MathUtils::IsEquivalent(m_indicies[15], rhs.m_indicies[15]));
}

bool Matrix4::operator==(const Matrix4& rhs) noexcept {
    return static_cast<const Matrix4&>(*this).operator==(rhs);
}

bool Matrix4::operator!=(const Matrix4& rhs) const noexcept {
    return !(*this == rhs);
}

bool Matrix4::operator!=(const Matrix4& rhs) noexcept {
    return static_cast<const Matrix4&>(*this).operator!=(rhs);
}

const float* Matrix4::GetAsFloatArray() const noexcept {
    return &m_indicies[0];
}
float* Matrix4::GetAsFloatArray() noexcept {
    return &m_indicies[0];
}
Vector3 Matrix4::GetTranslation() const noexcept {
    return Vector3(GetTBasis());
}
Vector3 Matrix4::GetTranslation() noexcept {
    return static_cast<const Matrix4&>(*this).GetTranslation();
}
Vector3 Matrix4::GetScale() const noexcept {
    return Vector3(GetIBasis().CalcLength3D(),
        GetJBasis().CalcLength3D(),
        GetKBasis().CalcLength3D());
}
Vector3 Matrix4::GetScale() noexcept {
    return static_cast<const Matrix4&>(*this).GetScale();
}

Matrix4 Matrix4::GetRotation() const noexcept {
    return Matrix4(GetIBasis(), GetJBasis(), GetKBasis());
}

Matrix4 Matrix4::GetRotation() noexcept {
    return static_cast<const Matrix4&>(*this).GetRotation();
}

Vector3 Matrix4::CalcEulerAngles() const noexcept {
    //Reference: http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.371.6578&rep=rep1&type=pdf

    auto z_comps = GetZComponents();
    auto z_i = z_comps.x;
    bool is_z_i_near_one = MathUtils::IsEquivalent(z_i, -1.0f) || MathUtils::IsEquivalent(z_i, 1.0f);
    if(is_z_i_near_one) {
        auto theta_1 = -std::asin(z_i);
        auto theta_2 = MathUtils::M_PI - theta_1;

        auto c_theta_1 = std::cos(theta_1);
        auto c_theta_2 = std::cos(theta_2);

        auto z_j = z_comps.y;
        auto z_k = z_comps.z;

        auto psi_1 = std::atan2(z_j / c_theta_1, z_k / c_theta_1);
        auto psi_2 = std::atan2(z_j / c_theta_2, z_k / c_theta_2);

        auto y_comps = GetYComponents();
        auto y_i = y_comps.x;

        auto x_comps = GetXComponents();
        auto x_i = x_comps.x;

        auto phi_1 = std::atan2(y_i / c_theta_1, x_i / c_theta_1);
        auto phi_2 = std::atan2(y_i / c_theta_2, x_i / c_theta_2);

        auto theta = (std::min)(theta_1, theta_2);
        auto psi = (std::min)(psi_1, psi_2);
        auto phi = (std::min)(phi_1, phi_2);
        return Vector3(psi, theta, phi);
    } else {
        auto x_comps = GetXComponents();
        auto x_j = x_comps.y;
        auto x_k = x_comps.z;
        auto phi = 0.0f;
        auto theta = 0.0f;
        auto psi = 0.0f;
        bool is_z_i_near_neg_one = MathUtils::IsEquivalent(z_i, -1.0f);
        if(is_z_i_near_neg_one) {
            theta = MathUtils::M_PI_2;
            psi = phi + std::atan2(x_j, x_k);
        } else {
            theta = -MathUtils::M_PI_2;
            psi = -phi + std::atan2(-x_j, -x_k);
        }
        return Vector3(psi, theta, phi);
    }
}

Matrix4 Matrix4::operator*(const Matrix4& rhs) const noexcept {

    using namespace MathUtils;

    Vector4 myI = GetIBasis();
    Vector4 myJ = GetJBasis();
    Vector4 myK = GetKBasis();
    Vector4 myT = GetTBasis();
    Vector4 myX = GetXComponents();
    Vector4 myY = GetYComponents();
    Vector4 myZ = GetZComponents();
    Vector4 myW = GetWComponents();

    Vector4 rhsI = rhs.GetIBasis();
    Vector4 rhsJ = rhs.GetJBasis();
    Vector4 rhsK = rhs.GetKBasis();
    Vector4 rhsT = rhs.GetTBasis();
    Vector4 rhsX = rhs.GetXComponents();
    Vector4 rhsY = rhs.GetYComponents();
    Vector4 rhsZ = rhs.GetZComponents();
    Vector4 rhsW = rhs.GetWComponents();

    float m00 = DotProduct(myX, rhsI);  float m01 = DotProduct(myX, rhsJ); float m02 = DotProduct(myX, rhsK);  float m03 = DotProduct(myX, rhsT);
    float m04 = DotProduct(myY, rhsI);  float m05 = DotProduct(myY, rhsJ); float m06 = DotProduct(myY, rhsK);  float m07 = DotProduct(myY, rhsT);
    float m08 = DotProduct(myZ, rhsI);  float m09 = DotProduct(myZ, rhsJ); float m10 = DotProduct(myZ, rhsK);  float m11 = DotProduct(myZ, rhsT);
    float m12 = DotProduct(myW, rhsI);  float m13 = DotProduct(myW, rhsJ); float m14 = DotProduct(myW, rhsK);  float m15 = DotProduct(myW, rhsT);

    Matrix4 result(m00, m01, m02, m03
        , m04, m05, m06, m07
        , m08, m09, m10, m11
        , m12, m13, m14, m15
    );
    return result;
}

Matrix4 Matrix4::operator*(float scalar) const noexcept {
    return Matrix4(scalar * m_indicies[0], scalar * m_indicies[1], scalar * m_indicies[2], scalar * m_indicies[3],
        scalar * m_indicies[4], scalar * m_indicies[5], scalar * m_indicies[6], scalar * m_indicies[7],
        scalar * m_indicies[8], scalar * m_indicies[9], scalar * m_indicies[10], scalar * m_indicies[11],
        scalar * m_indicies[12], scalar * m_indicies[13], scalar * m_indicies[14], scalar * m_indicies[15]);
}

Vector4 Matrix4::operator*(const Vector4& rhs) const noexcept {
    const Vector4 my_x{ GetXComponents() };
    const Vector4 my_y{ GetYComponents() };
    const Vector4 my_z{ GetZComponents() };
    const Vector4 my_w{ GetWComponents() };
    const float x = MathUtils::DotProduct(rhs, my_x);
    const float y = MathUtils::DotProduct(rhs, my_y);
    const float z = MathUtils::DotProduct(rhs, my_z);
    const float w = MathUtils::DotProduct(rhs, my_w);
    return Vector4(x, y, z, w);
}

Vector4 operator*(const Vector4& lhs, const Matrix4& rhs) noexcept {
    const Vector4 my_i(rhs.GetIBasis());
    const Vector4 my_j(rhs.GetJBasis());
    const Vector4 my_k(rhs.GetKBasis());
    const Vector4 my_t(rhs.GetTBasis());
    const float x = MathUtils::DotProduct(lhs, my_i);
    const float y = MathUtils::DotProduct(lhs, my_j);
    const float z = MathUtils::DotProduct(lhs, my_k);
    const float w = MathUtils::DotProduct(lhs, my_t);
    return Vector4(x, y, z, w);
}

Vector3 Matrix4::operator*(const Vector3& rhs) const noexcept {
    const Vector3 my_x(GetXComponents());
    const Vector3 my_y(GetYComponents());
    const Vector3 my_z(GetZComponents());
    const float x = MathUtils::DotProduct(my_x, rhs);
    const float y = MathUtils::DotProduct(my_y, rhs);
    const float z = MathUtils::DotProduct(my_z, rhs);
    return Vector3(x, y, z);
}

Vector3 operator*(const Vector3& lhs, const Matrix4& rhs) noexcept {
    const Vector3 my_i(rhs.GetIBasis());
    const Vector3 my_j(rhs.GetJBasis());
    const Vector3 my_k(rhs.GetKBasis());
    const float x = MathUtils::DotProduct(lhs, my_i);
    const float y = MathUtils::DotProduct(lhs, my_j);
    const float z = MathUtils::DotProduct(lhs, my_k);
    return Vector3(x, y, z);
}

Vector2 Matrix4::operator*(const Vector2& rhs) const noexcept {
    const Vector2 my_x{ Vector3{ GetXComponents() } };
    const Vector2 my_y{ Vector3{ GetYComponents() } };
    const float x = MathUtils::DotProduct(my_x, rhs);
    const float y = MathUtils::DotProduct(my_y, rhs);
    return Vector2(x, y);
}

Vector2 operator*(const Vector2& lhs, const Matrix4& rhs) noexcept {
    const Vector2 my_i{ Vector3{ rhs.GetIBasis() } };
    const Vector2 my_j{ Vector3{ rhs.GetKBasis() } };
    const float x = MathUtils::DotProduct(lhs, my_i);
    const float y = MathUtils::DotProduct(lhs, my_j);
    return Vector2(x, y);
}

Matrix4& Matrix4::operator*=(const Matrix4& rhs) noexcept {

    using namespace MathUtils;

    Vector4 myI = GetIBasis();
    Vector4 myJ = GetJBasis();
    Vector4 myK = GetKBasis();
    Vector4 myT = GetTBasis();
    Vector4 myX = GetXComponents();
    Vector4 myY = GetYComponents();
    Vector4 myZ = GetZComponents();
    Vector4 myW = GetWComponents();

    Vector4 rhsI = rhs.GetIBasis();
    Vector4 rhsJ = rhs.GetJBasis();
    Vector4 rhsK = rhs.GetKBasis();
    Vector4 rhsT = rhs.GetTBasis();
    Vector4 rhsX = rhs.GetXComponents();
    Vector4 rhsY = rhs.GetYComponents();
    Vector4 rhsZ = rhs.GetZComponents();
    Vector4 rhsW = rhs.GetWComponents();


    m_indicies[0] = DotProduct(myX, rhsI);  m_indicies[1] = DotProduct(myX, rhsJ); m_indicies[2] = DotProduct(myX, rhsK);  m_indicies[3] = DotProduct(myX, rhsT);
    m_indicies[4] = DotProduct(myY, rhsI);  m_indicies[5] = DotProduct(myY, rhsJ); m_indicies[6] = DotProduct(myY, rhsK); m_indicies[7] = DotProduct(myY, rhsT);
    m_indicies[8] = DotProduct(myZ, rhsI);  m_indicies[9] = DotProduct(myZ, rhsJ); m_indicies[10] = DotProduct(myZ, rhsK);  m_indicies[11] = DotProduct(myZ, rhsT);
    m_indicies[12] = DotProduct(myW, rhsI);  m_indicies[13] = DotProduct(myW, rhsJ); m_indicies[14] = DotProduct(myW, rhsK);  m_indicies[15] = DotProduct(myW, rhsT);

    return *this;
}

Matrix4& Matrix4::operator*=(float scalar) noexcept {

    m_indicies[0] *= scalar;
    m_indicies[1] *= scalar;
    m_indicies[2] *= scalar;
    m_indicies[3] *= scalar;

    m_indicies[4] *= scalar;
    m_indicies[5] *= scalar;
    m_indicies[6] *= scalar;
    m_indicies[7] *= scalar;

    m_indicies[8] *= scalar;
    m_indicies[9] *= scalar;
    m_indicies[10] *= scalar;
    m_indicies[11] *= scalar;

    m_indicies[12] *= scalar;
    m_indicies[13] *= scalar;
    m_indicies[14] *= scalar;
    m_indicies[15] *= scalar;

    return *this;
}

const float * Matrix4::operator*() const noexcept {
    return &m_indicies[0];
}
float* Matrix4::operator*() noexcept {
    return &m_indicies[0];
}

Matrix4 Matrix4::operator+(const Matrix4& rhs) const noexcept {
    return Matrix4(m_indicies[0] + rhs.m_indicies[0], m_indicies[1] + rhs.m_indicies[1], m_indicies[2] + rhs.m_indicies[2], m_indicies[3] + rhs.m_indicies[3],
        m_indicies[4] + rhs.m_indicies[4], m_indicies[5] + rhs.m_indicies[5], m_indicies[6] + rhs.m_indicies[6], m_indicies[7] + rhs.m_indicies[7],
        m_indicies[8] + rhs.m_indicies[8], m_indicies[9] + rhs.m_indicies[9], m_indicies[10] + rhs.m_indicies[10], m_indicies[11] + rhs.m_indicies[11],
        m_indicies[12] + rhs.m_indicies[12], m_indicies[13] + rhs.m_indicies[13], m_indicies[14] + rhs.m_indicies[14], m_indicies[15] + rhs.m_indicies[15]);
}

Matrix4& Matrix4::operator+=(const Matrix4& rhs) noexcept {

    m_indicies[0] += rhs.m_indicies[0];
    m_indicies[1] += rhs.m_indicies[1];
    m_indicies[2] += rhs.m_indicies[2];
    m_indicies[3] += rhs.m_indicies[3];

    m_indicies[4] += rhs.m_indicies[4];
    m_indicies[5] += rhs.m_indicies[5];
    m_indicies[6] += rhs.m_indicies[6];
    m_indicies[7] += rhs.m_indicies[7];

    m_indicies[8] += rhs.m_indicies[8];
    m_indicies[9] += rhs.m_indicies[9];
    m_indicies[10] += rhs.m_indicies[10];
    m_indicies[11] += rhs.m_indicies[11];

    m_indicies[12] += rhs.m_indicies[12];
    m_indicies[13] += rhs.m_indicies[13];
    m_indicies[14] += rhs.m_indicies[14];
    m_indicies[15] += rhs.m_indicies[15];

    return *this;
}

Matrix4 Matrix4::operator-(const Matrix4& rhs) const noexcept {
    return Matrix4(m_indicies[0] - rhs.m_indicies[0], m_indicies[1] - rhs.m_indicies[1], m_indicies[2] - rhs.m_indicies[2], m_indicies[3] - rhs.m_indicies[3],
        m_indicies[4] - rhs.m_indicies[4], m_indicies[5] - rhs.m_indicies[5], m_indicies[6] - rhs.m_indicies[6], m_indicies[7] - rhs.m_indicies[7],
        m_indicies[8] - rhs.m_indicies[8], m_indicies[9] - rhs.m_indicies[9], m_indicies[10] - rhs.m_indicies[10], m_indicies[11] - rhs.m_indicies[11],
        m_indicies[12] - rhs.m_indicies[12], m_indicies[13] - rhs.m_indicies[13], m_indicies[14] - rhs.m_indicies[14], m_indicies[15] - rhs.m_indicies[15]);
}

Matrix4& Matrix4::operator-=(const Matrix4& rhs) noexcept {

    m_indicies[0] -= rhs.m_indicies[0];
    m_indicies[1] -= rhs.m_indicies[1];
    m_indicies[2] -= rhs.m_indicies[2];
    m_indicies[3] -= rhs.m_indicies[3];

    m_indicies[4] -= rhs.m_indicies[4];
    m_indicies[5] -= rhs.m_indicies[5];
    m_indicies[6] -= rhs.m_indicies[6];
    m_indicies[7] -= rhs.m_indicies[7];

    m_indicies[8] -= rhs.m_indicies[8];
    m_indicies[9] -= rhs.m_indicies[9];
    m_indicies[10] -= rhs.m_indicies[10];
    m_indicies[11] -= rhs.m_indicies[11];

    m_indicies[12] -= rhs.m_indicies[12];
    m_indicies[13] -= rhs.m_indicies[13];
    m_indicies[14] -= rhs.m_indicies[14];
    m_indicies[15] -= rhs.m_indicies[15];

    return *this;
}

Matrix4 Matrix4::operator/(const Matrix4& rhs) noexcept {
    return Matrix4((*this) * Matrix4::CalculateInverse(rhs));
}

Matrix4& Matrix4::operator/=(const Matrix4& rhs) noexcept {

    Matrix4 inv(Matrix4::CalculateInverse(rhs));
    Matrix4 result = (*this * inv);

    m_indicies = result.m_indicies;

    return *this;
}

Matrix4 Matrix4::operator-() const noexcept {
    return Matrix4(-GetIBasis(), -GetJBasis(), -GetKBasis(), -GetTBasis());
}

float& Matrix4::operator[](std::size_t index) {
    return m_indicies[index];
}

const float& Matrix4::operator[](std::size_t index) const {
    return m_indicies[index];
}

Matrix4 operator*(float lhs, const Matrix4& rhs) noexcept {
    return Matrix4(lhs * rhs.m_indicies[0], lhs * rhs.m_indicies[1], lhs * rhs.m_indicies[2], lhs * rhs.m_indicies[3],
        lhs * rhs.m_indicies[4], lhs * rhs.m_indicies[5], lhs * rhs.m_indicies[6], lhs * rhs.m_indicies[7],
        lhs * rhs.m_indicies[8], lhs * rhs.m_indicies[9], lhs * rhs.m_indicies[10], lhs * rhs.m_indicies[11],
        lhs * rhs.m_indicies[12], lhs * rhs.m_indicies[13], lhs * rhs.m_indicies[14], lhs * rhs.m_indicies[15]);
}

std::ostream& operator<<(std::ostream& out_stream, const Matrix4& m) noexcept {
    out_stream << '[' << m.m_indicies[0] << ',' << m.m_indicies[1] << ',' << m.m_indicies[2] << ',' << m.m_indicies[3] << ','
        << m.m_indicies[4] << ',' << m.m_indicies[5] << ',' << m.m_indicies[6] << ',' << m.m_indicies[7] << ','
        << m.m_indicies[8] << ',' << m.m_indicies[9] << ',' << m.m_indicies[10] << ',' << m.m_indicies[11] << ','
        << m.m_indicies[12] << ',' << m.m_indicies[13] << ',' << m.m_indicies[14] << ',' << m.m_indicies[15] << ']';
    return out_stream;
}

std::istream& operator>>(std::istream& in_stream, Matrix4& m) noexcept {

    std::array<float, 16> indicies;

    in_stream.ignore(); //[
    in_stream >> indicies[0];
    in_stream.ignore(); //,
    in_stream >> indicies[1];
    in_stream.ignore(); //,
    in_stream >> indicies[2];
    in_stream.ignore(); //,
    in_stream >> indicies[3];
    in_stream.ignore(); //,
    in_stream >> indicies[4];
    in_stream.ignore(); //,
    in_stream >> indicies[5];
    in_stream.ignore(); //,
    in_stream >> indicies[6];
    in_stream.ignore(); //,
    in_stream >> indicies[7];
    in_stream.ignore(); //,
    in_stream >> indicies[8];
    in_stream.ignore(); //,
    in_stream >> indicies[9];
    in_stream.ignore(); //,
    in_stream >> indicies[10];
    in_stream.ignore(); //,
    in_stream >> indicies[11];
    in_stream.ignore(); //,
    in_stream >> indicies[12];
    in_stream.ignore(); //,
    in_stream >> indicies[13];
    in_stream.ignore(); //,
    in_stream >> indicies[14];
    in_stream.ignore(); //,
    in_stream >> indicies[15];
    in_stream.ignore(); //]

    m.m_indicies = indicies;

    return in_stream;
}
