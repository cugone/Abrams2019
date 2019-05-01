#include "Engine/Math/Quaternion.hpp"

#include <cmath>
#include <sstream>

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"

Quaternion Quaternion::I = Quaternion(1.0f, Vector3::ZERO);

Quaternion::Quaternion(const Matrix4& mat)
    : w(1.0f)
    , axis(Vector3::ZERO) {
    //From http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

    Vector4 diag = mat.GetDiagonal();
    float trace = mat.CalculateTrace();

    Vector3 row_zero = Vector3(mat.GetIBasis());
    Vector3 row_one = Vector3(mat.GetJBasis());
    Vector3 row_two = Vector3(mat.GetKBasis());
    Vector3 row_three = Vector3(mat.GetTBasis());

    if(trace > 0.0f) {

        float S = 0.5f / std::sqrt(trace);

        w = 0.25f / S;

        axis.x = (row_two.y - row_one.z)  * S;
        axis.y = (row_zero.z - row_two.x) * S;
        axis.z = (row_one.x - row_zero.y) * S;

    } else {

        int i = 0;
        if(row_one.y > row_zero.x) i = 1;
        if(row_two.z > mat.GetIndex(i, i)) i = 2;

        const int next[3] = { 1, 2, 0 };
        int j = next[i];
        int k = next[j];

        float S = std::sqrt((mat.GetIndex(i, i) - (mat.GetIndex(j, j) + mat.GetIndex(k, k))) + 1.0f);

        float* axisAsFloats = axis.GetAsFloatArray();

        axisAsFloats[i] = S * 0.5f;

        float t = 0.0f;
        if(!MathUtils::IsEquivalent(S, 0.0f)) {
            t = 0.5f / S;
        } else {
            t = S;
        }
        w = (mat.GetIndex(k, j) - mat.GetIndex(j, k)) * t;
        axisAsFloats[j] = (mat.GetIndex(j, i) + mat.GetIndex(i, j)) * t;
        axisAsFloats[k] = (mat.GetIndex(k, i) + mat.GetIndex(i, k)) * t;
        switch(i) {
            case 0:
                axis.x = axisAsFloats[i];
                break;
            case 1:
                axis.y = axisAsFloats[i];
                break;
            case 2:
                axis.z = axisAsFloats[i];
                break;
        }
        switch(j) {
            case 0:
                axis.x = axisAsFloats[j];
                break;
            case 1:
                axis.y = axisAsFloats[j];
                break;
            case 2:
                axis.z = axisAsFloats[j];
                break;
        }
        switch(k) {
            case 0:
                axis.x = axisAsFloats[k];
                break;
            case 1:
                axis.y = axisAsFloats[k];
                break;
            case 2:
                axis.z = axisAsFloats[k];
                break;
        }
    }
    if(!MathUtils::IsEquivalent(CalcLengthSquared(), 1.0f)) {
        Normalize();
    }
}

Quaternion::Quaternion(float initialScalar, const Vector3& initialAxis)
    : w(initialScalar)
    , axis(initialAxis) {
    if(!MathUtils::IsEquivalent(CalcLengthSquared(), 1.0f)) {
        Normalize();
    }
}

Quaternion::Quaternion(float initialW, float initialX, float initialY, float initialZ)
    : w(initialW)
    , axis(initialX, initialY, initialZ) {
    if(!MathUtils::IsEquivalent(CalcLengthSquared(), 1.0f)) {
        Normalize();
    }
}

Quaternion::Quaternion(const Vector3& rotations)
    : w(0.0f)
    , axis(rotations) {
    if(!MathUtils::IsEquivalent(CalcLengthSquared(), 1.0f)) {
        Normalize();
    }
}

Quaternion::Quaternion(const std::string& value)
    : w(0.0f)
    , axis() {
    if(value[0] == '[') {
        if(value.back() == ']') {
            std::stringstream ss(value.substr(1, value.size() - 1));
            std::string curLine;
            for(int i = 0; std::getline(ss, curLine, ','); ++i) {
                switch(i) {
                    case 0: w = std::stof(curLine); break;
                    case 1: axis.x = std::stof(curLine); break;
                    case 2: axis.y = std::stof(curLine); break;
                    case 3: axis.z = std::stof(curLine); break;
                }
            }
        }
    }
    if(!MathUtils::IsEquivalent(CalcLengthSquared(), 1.0f)) {
        Normalize();
    }
}
Quaternion::~Quaternion() {
    /* DO NOTHING */
}

Quaternion  Quaternion::operator+(const Quaternion& rhs) const {
    return Quaternion(this->w + rhs.w, this->axis + rhs.axis);
}
Quaternion& Quaternion::operator+=(const Quaternion& rhs) {
    this->w += rhs.w;
    this->axis += rhs.axis;
    return *this;
}
Quaternion  Quaternion::operator-(const Quaternion& rhs) const {
    return Quaternion(this->w - rhs.w, this->axis - rhs.axis);
}
Quaternion& Quaternion::operator-=(const Quaternion& rhs) {
    this->w -= rhs.w;
    this->axis -= rhs.axis;
    return *this;
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const {
    return Quaternion(this->w * rhs.w - MathUtils::DotProduct(this->axis, rhs.axis),
                      this->w * rhs.axis + rhs.w * this->axis + MathUtils::CrossProduct(this->axis, rhs.axis));
}
Quaternion& Quaternion::operator*=(const Quaternion& rhs) {
    this->w = this->w * rhs.w - MathUtils::DotProduct(this->axis, rhs.axis);
    this->axis = this->w * rhs.axis + rhs.w * this->axis + MathUtils::CrossProduct(this->axis, rhs.axis);
    return *this;
}

Quaternion Quaternion::operator*(float scalar) const {
    return Quaternion(this->w * scalar, this->axis * scalar);
}
Quaternion& Quaternion::operator*=(float scalar) {
    this->w *= scalar;
    this->axis *= axis;
    return *this;
}

Quaternion Quaternion::operator*(const Vector3& rhs) const {
    return *this * Quaternion(rhs);
}
Quaternion& Quaternion::operator*=(const Vector3& rhs) {
    return this->operator*=(Quaternion(rhs));
}
Quaternion Quaternion::operator-() {
    return Quaternion(-this->w, -this->axis);
}


bool Quaternion::operator==(const Quaternion& rhs) const {
    return w == rhs.w && axis == rhs.axis;
}

bool Quaternion::operator!=(const Quaternion& rhs) const {
    return !(*this == rhs);
}

Vector4 Quaternion::CalcAxisAngles(bool degrees) const {
    if(degrees) {
        return CalcAxisAnglesDegrees();
    } else {
        return CalcAxisAnglesRadians();
    }
}

Vector4 Quaternion::CalcAxisAnglesDegrees() const {
    auto q_n = GetNormalize();
    if(q_n.w > 1.0f) {
        q_n.Normalize();
    }
    float s = std::sqrt(1.0f - q_n.w * q_n.w);
    float angle = 2.0f * std::acos(q_n.w);
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    if(MathUtils::IsEquivalent(s, 0.0f)) {
        x = q_n.axis.x;
        y = q_n.axis.y;
        z = q_n.axis.z;
    } else {
        x = q_n.axis.x / s;
        y = q_n.axis.y / s;
        z = q_n.axis.z / s;
    }
    return Vector4(x, y, z, MathUtils::ConvertRadiansToDegrees(angle));
}

Vector4 Quaternion::CalcAxisAnglesRadians() const {
    auto q_n = GetNormalize();
    if(q_n.w > 1.0f) {
        q_n.Normalize();
    }
    float s = std::sqrt(1.0f - q_n.w * q_n.w);
    float angle = 2.0f * std::acos(q_n.w);
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    if(MathUtils::IsEquivalent(s, 0.0f)) {
        x = q_n.axis.x;
        y = q_n.axis.y;
        z = q_n.axis.z;
    } else {
        x = q_n.axis.x / s;
        y = q_n.axis.y / s;
        z = q_n.axis.z / s;
    }
    return Vector4(x, y, z, angle);
}

Vector3 Quaternion::CalcEulerAnglesDegrees() const {
    return CalcEulerAngles(true);
}

Vector3 Quaternion::CalcEulerAnglesRadians() const {
    return CalcEulerAngles(false);
}

//From http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
Vector3 Quaternion::CalcEulerAngles(bool degrees) const {

    //First pass change to non-normalized version later.

    auto q_n = GetNormalize();

    //Euclidean Space standards
    //Heading = rot. about y (yaw)
    //Attitude = rot. about z (roll)
    //Bank = rot. about x (pitch)
    //HAB order (yaw roll pitch)
    //float c1 = std::cos(heading);
    //float c2 = std::cos(attitude);
    //float c3 = std::cos(bank);
    //float s1 = std::sin(heading);
    //float s2 = std::sin(attitude);
    //float s3 = std::sin(bank);

    Vector3 result;
    float test = q_n.axis.x * q_n.axis.y + q_n.axis.z * q_n.w;
    float y = 2.0f * std::atan2(q_n.axis.x, q_n.w);
    float z = MathUtils::M_1PI_2;
    float x = 0.0f;
    if(MathUtils::IsEquivalent(test, 0.50f)) {
        result.y = y;
        result.z = z;
        result.x = x;
        return result;
    } else if(MathUtils::IsEquivalent(test, -0.50f)) {
        result.y = -y;
        result.z = -z;
        result.x = x;
        return result;
    }

    float sqx = q_n.axis.x * q_n.axis.x;
    float sqy = q_n.axis.y * q_n.axis.y;
    float sqz = q_n.axis.z * q_n.axis.z;
    y = std::atan2(2.0f * q_n.axis.y * w - 2.0f * q_n.axis.x * q_n.axis.z, 1.0f - sqy - 2.0f * sqz);
    z = std::asin(2.0f * test);
    x = std::atan2(2.0f * q_n.axis.x * w - 2.0f * q_n.axis.z, 1.0f - 2.0f * sqx - 2.0f * sqz);

    if(degrees) {
        result.y = MathUtils::ConvertRadiansToDegrees(y);
        result.x = MathUtils::ConvertRadiansToDegrees(x);
        result.z = MathUtils::ConvertRadiansToDegrees(z);
    } else {
        result.y = y;
        result.x = x;
        result.z = z;
    }
    return result;
}

float Quaternion::CalcLength() const {
    return std::sqrt(CalcLengthSquared());
}

float Quaternion::CalcLengthSquared() const {
    return w * w + axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
}

void Quaternion::Normalize() {
    float lengthSq = CalcLengthSquared();
    if(!MathUtils::IsEquivalent(lengthSq, 0.0f)) {
        float invLength = 1.0f / std::sqrt(lengthSq);
        this->w *= invLength;
        this->axis *= invLength;
    }
}

Quaternion Quaternion::GetNormalize() const {
    Quaternion q = *this;
    q.Normalize();
    return q;
}

void Quaternion::Conjugate() {
    this->axis = -this->axis;
}

Quaternion Quaternion::GetConjugate() const {
    Quaternion q = *this;
    q.Conjugate();
    return q;
}

void Quaternion::Inverse() {
    float lengthSq = CalcLengthSquared();
    if(!MathUtils::IsEquivalent(lengthSq, 0.0f)) {
        Quaternion q_conj = *this;
        q_conj.Conjugate();
        float invLengthSq = 1.0f / lengthSq;
        Quaternion result = q_conj * invLengthSq;
        this->w = result.w;
        this->axis = result.axis;
    }
}

Quaternion Quaternion::CalcInverse() const {
    float lengthSq = CalcLengthSquared();
    Quaternion result;
    if(!MathUtils::IsEquivalent(lengthSq, 0.0f)) {
        Quaternion q_conj = *this;
        q_conj.Conjugate();
        float invLengthSq = 1.0f / lengthSq;
        result = q_conj * invLengthSq;
    }
    return result;
}

Quaternion Quaternion::CreateRealQuaternion(float scalar) {
    return Quaternion(scalar, Vector3::ZERO);
}
Quaternion Quaternion::CreatePureQuaternion(const Vector3& v) {
    return Quaternion(0.0f, v.GetNormalize());
}

Quaternion Quaternion::CreateFromAxisAngle(const Vector3& axis, float degreesAngle) {
    float angle = MathUtils::ConvertDegreesToRadians(degreesAngle);
    float factor = std::sin(angle * 0.5f);

    Vector3 factoredAxis = axis.GetNormalize() * factor;
    float w = std::cos(angle * 0.5f);

    return Quaternion(w, factoredAxis);
}

Quaternion Quaternion::CreateFromEulerAnglesDegrees(float pitch, float yaw, float roll) {
    return CreateFromEulerAngles(pitch, yaw, roll, true);
}

Quaternion Quaternion::CreateFromEulerAnglesRadians(float pitch, float yaw, float roll) {
    return CreateFromEulerAngles(pitch, yaw, roll, false);
}

Quaternion Quaternion::CreateFromEulerAngles(float pitch, float yaw, float roll, bool degrees) {
    //quaternion quaternion::FromEuler(vec3 const &euler) {
    //    // If this changes - this method is no longer valid
    //    ASSERT(ROTATE_DEFAULT == ROTATE_ZXY);
    //    vec3 const he = .5f * euler;
    //    float cx = cosf(he.x);
    //    float sx = sinf(he.x);
    //    float cy = cosf(he.y);
    //    float sy = sinf(he.y);
    //    float cz = cosf(he.z);
    //    float sz = sinf(he.z);
    //    float r = cx*cy*cz + sx*sy*sz;
    //    float ix = sx*cy*cz + cx*sy*sz;
    //    float iy = cx*sy*cz - sx*cy*sz;
    //    float iz = cx*cy*sz - sx*sy*cz;

    //    quaternion q = quaternion(r, ix, iy, iz);
    //    q.normalize();

    //    // ASSERT( q.is_unit() );
    //    return q;
    //}

    if(degrees) {
        yaw = MathUtils::ConvertDegreesToRadians(yaw);
        pitch = MathUtils::ConvertDegreesToRadians(pitch);
        roll = MathUtils::ConvertDegreesToRadians(roll);
    }

    //Euclidean Space standards
    //Heading = rot. about y (yaw)
    //Attitude = rot. about z (roll)
    //Bank = rot. about x (pitch)
    //HAB order (yaw roll pitch)

    //    vec3 const he = .5f * euler;
    //    float cx = cosf(he.x);
    //    float sx = sinf(he.x);
    //    float cy = cosf(he.y);
    //    float sy = sinf(he.y);
    //    float cz = cosf(he.z);
    //    float sz = sinf(he.z);

    //    float r = cx*cy*cz + sx*sy*sz;
    //    float ix = sx*cy*cz + cx*sy*sz;
    //    float iy = cx*sy*cz - sx*cy*sz;
    //    float iz = cx*cy*sz - sx*sy*cz;

    Vector3 he = Vector3(pitch, yaw, roll) * 0.5f;
    float cx = std::cos(he.x);
    float sx = std::sin(he.x);
    float cy = std::cos(he.y);
    float sy = std::sin(he.y);
    float cz = std::cos(he.z);
    float sz = std::sin(he.z);

    float w = cx * cy*cz - sx * sy*sz;
    float ix = sx * cy*cz + cx * sy*sz;
    float iy = cx * sy*cz + sx * cy*sz;
    float iz = cx * cy*sz - sx * sy*cz;

    Quaternion result(w, ix, iy, iz);

    return result;
}

Quaternion Quaternion::GetIdentity() {
    return Quaternion(1.0f, Vector3::ZERO);
}
Quaternion Conjugate(const Quaternion& q) {
    return Quaternion(q.w, -q.axis);
}

Quaternion Inverse(const Quaternion& q) {
    float lengthSq = q.CalcLengthSquared();
    if(!MathUtils::IsEquivalent(lengthSq, 0.0f)) {
        Quaternion q_conj = q;
        q_conj.Conjugate();
        float invLengthSq = 1.0f / lengthSq;
        Quaternion result = q_conj * invLengthSq;
        return result;
    }
    return q;
}

Quaternion operator*(float scalar, const Quaternion& rhs) {
    return Quaternion(scalar * rhs.w, scalar * rhs.axis);
}

Quaternion& operator*=(float scalar, Quaternion& rhs) {
    rhs.w *= scalar;
    rhs.axis *= scalar;
    return rhs;
}

Quaternion operator*(const Vector3& lhs, const Quaternion& rhs) {
    return Quaternion(Quaternion(lhs) * rhs);
}

Quaternion& operator*=(const Vector3& lhs, Quaternion& rhs) {
    rhs = Quaternion(lhs) * rhs;
    return rhs;
}
