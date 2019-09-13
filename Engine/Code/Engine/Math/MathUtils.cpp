#include "Engine/Math/MathUtils.hpp"

#include <cmath>

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/Capsule3.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Sphere3.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Quaternion.hpp"

namespace MathUtils {

namespace {
static thread_local unsigned int MT_RANDOM_SEED = 0u;
}

void SetRandomEngineSeed(unsigned int seed) noexcept {
    MT_RANDOM_SEED = seed;
}

std::pair<float, float> SplitFloatingPointValue(float value) noexcept {
    float frac = 0.0f;
    float int_part = 0.0f;
    frac = std::modf(value, &int_part);
    return std::make_pair(int_part, frac);
}

std::pair<double, double> SplitFloatingPointValue(double value) noexcept {
    double frac = 0.0;
    double int_part = 0.0;
    frac = std::modf(value, &int_part);
    return std::make_pair(int_part, frac);
}

std::pair<long double, long double> SplitFloatingPointValue(long double value) noexcept {
    long double frac = 0.0;
    long double int_part = 0.0;
    frac = std::modf(value, &int_part);
    return std::make_pair(int_part, frac);
}

float ConvertDegreesToRadians(float degrees) noexcept {
    return degrees * (MathUtils::M_PI / 180.0f);
}

float ConvertRadiansToDegrees(float radians) noexcept {
    return radians * (180.0f * MathUtils::M_1_PI);
}

std::random_device& GetRandomDevice() noexcept {
    static thread_local std::random_device rd;
    return rd;
}

std::mt19937& GetMTRandomEngine(unsigned int seed /*= 0*/) noexcept {
    static thread_local std::mt19937 e = std::mt19937(!seed ? GetRandomDevice()() : seed);
    return e;
}

std::mt19937_64& GetMT64RandomEngine(unsigned int seed /*= 0*/) noexcept {
    static thread_local std::mt19937_64 e = std::mt19937_64(!seed ? GetRandomDevice()() : seed);
    return e;
}

bool GetRandomBool() noexcept {
    return MathUtils::GetRandomIntLessThan(2) == 0;
}

int GetRandomIntLessThan(int maxValueNotInclusive) noexcept {
    std::uniform_int_distribution<int> d(0, maxValueNotInclusive - 1);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

int GetRandomIntInRange(int minInclusive, int maxInclusive) noexcept {
    std::uniform_int_distribution<int> d(minInclusive, maxInclusive);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));

}

long GetRandomLongLessThan(long maxValueNotInclusive) noexcept {
    std::uniform_int_distribution<long> d(0L, maxValueNotInclusive - 1L);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

long GetRandomLongInRange(long minInclusive, long maxInclusive) noexcept {
    std::uniform_int_distribution<long> d(minInclusive, maxInclusive);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

long long GetRandomLongLongLessThan(long long maxValueNotInclusive) noexcept {
    std::uniform_int_distribution<long long> d(0LL, maxValueNotInclusive - 1LL);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

long long GetRandomLongLongInRange(long long minInclusive, long long maxInclusive) noexcept {
    std::uniform_int_distribution<long long> d(minInclusive, maxInclusive);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

float GetRandomFloatInRange(float minInclusive, float maxInclusive) noexcept {
    std::uniform_real_distribution<float> d(minInclusive, std::nextafter(maxInclusive, maxInclusive + 1.0f));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

float GetRandomFloatZeroToOne() noexcept {
    std::uniform_real_distribution<float> d(0.0f, std::nextafter(1.0f, 2.0f));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

float GetRandomFloatZeroUpToOne() noexcept {
    std::uniform_real_distribution<float> d(0.0f, 1.0f);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

float GetRandomFloatNegOneToOne() noexcept {
    return GetRandomFloatInRange(-1.0f, 1.0f);
}

double GetRandomDoubleInRange(double minInclusive, double maxInclusive) noexcept {
    std::uniform_real_distribution<double> d(minInclusive, std::nextafter(maxInclusive, maxInclusive + 1.0));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

double GetRandomDoubleZeroToOne() noexcept {
    std::uniform_real_distribution<double> d(0.0, std::nextafter(1.0, 2.0));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

double GetRandomDoubleZeroUpToOne() noexcept {
    std::uniform_real_distribution<double> d(0.0, 1.0);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

double GetRandomDoubleNegOneToOne() noexcept {
    return GetRandomDoubleInRange(-1.0, 1.0);
}

long double GetRandomLongDoubleInRange(long double minInclusive, long double maxInclusive) noexcept {
    std::uniform_real_distribution<long double> d(minInclusive, std::nextafter(maxInclusive, maxInclusive + 1.0L));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

long double GetRandomLongDoubleZeroToOne() noexcept {
    std::uniform_real_distribution<long double> d(0.0L, std::nextafter(1.0L, 2.0L));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

long double GetRandomLongDoubleZeroUpToOne() noexcept {
    std::uniform_real_distribution<long double> d(0.0L, 1.0L);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

long double GetRandomLongDoubleNegOneToOne() noexcept {
    return GetRandomDoubleInRange(-1.0L, 1.0L);
}

bool IsPercentChance(float probability) noexcept {
    float roll = GetRandomFloatZeroToOne();
    return roll < probability;
}

bool IsPercentChance(double probability) noexcept {
    double roll = GetRandomDoubleZeroToOne();
    return roll < probability;
}

bool IsPercentChance(long double probability) noexcept {
    long double roll = GetRandomLongDoubleZeroToOne();
    return roll < probability;
}

float CosDegrees(float degrees) noexcept {
    float radians = MathUtils::ConvertDegreesToRadians(degrees);
    return std::cos(radians);
}

float SinDegrees(float degrees) noexcept {
    float radians = MathUtils::ConvertDegreesToRadians(degrees);
    return std::sin(radians);
}

float Atan2Degrees(float y, float x) noexcept {
    float radians = std::atan2(y, x);
    return MathUtils::ConvertRadiansToDegrees(radians);
}

bool IsEquivalent(float a, float b, float epsilon /*= 0.00001f*/) noexcept {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(double a, double b, double epsilon /*= 0.0001*/) noexcept {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(long double a, long double b, long double epsilon /*= 0.0001L*/) noexcept {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(const Vector2& a, const Vector2& b, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon);
}

bool IsEquivalent(const Vector3& a, const Vector3& b, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon) && IsEquivalent(a.z, b.z, epsilon);
}

bool IsEquivalent(const Vector4& a, const Vector4& b, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon) && IsEquivalent(a.z, b.z, epsilon) && IsEquivalent(a.w, b.w, epsilon);
}

bool IsEquivalent(const Quaternion& a, const Quaternion& b, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a.w, b.w, epsilon) && IsEquivalent(a.axis, b.axis, epsilon);
}

bool IsEquivalentOrLessThan(float a, float b, float epsilon /*= 0.00001f*/) noexcept {
    return a < b || IsEquivalent(a, b, epsilon);
}

bool IsEquivalentOrLessThan(double a, double b, double epsilon /*= 0.0001*/) noexcept {
    return a < b || IsEquivalent(a, b, epsilon);
}

bool IsEquivalentOrLessThan(long double a, long double b, long double epsilon /*= 0.0001L*/) noexcept {
    return a < b || IsEquivalent(a, b, epsilon);
}

bool IsEquivalentToZero(float a, float epsilon /*= 0.00001f*/) noexcept {
    return IsEquivalent(a, 0.0f, epsilon);
}

bool IsEquivalentToZero(double a, double epsilon /*= 0.0001*/) noexcept {
    return IsEquivalent(a, 0.0, epsilon);
}

bool IsEquivalentToZero(long double a, long double epsilon /*= 0.0001L*/) noexcept {
    return IsEquivalent(a, 0.0L, epsilon);
}

bool IsEquivalentToZero(const Vector2& a, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a, Vector2::ZERO, epsilon);
}

bool IsEquivalentToZero(const Vector3& a, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a, Vector3::ZERO, epsilon);
}

bool IsEquivalentToZero(const Vector4& a, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a, Vector4::ZERO, epsilon);
}

bool IsEquivalentToZero(const Quaternion& a, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a, Quaternion::GetIdentity(), epsilon);
}

float CalcDistance(const Vector2& a, const Vector2& b) noexcept {
    return (b - a).CalcLength();
}

float CalcDistance(const Vector3& a, const Vector3& b) noexcept {
    return (b - a).CalcLength();
}

float CalcDistance(const Vector4& a, const Vector4& b) noexcept {
    return (b - a).CalcLength4D();
}

float CalcDistance(const Vector2& p, const LineSegment2& line) noexcept {
    return std::sqrt(CalcDistanceSquared(p, line));
}

float CalcDistance(const Vector3& p, const LineSegment3& line) noexcept {
    return std::sqrt(CalcDistanceSquared(p, line));
}

float CalcDistance4D(const Vector4& a, const Vector4& b) noexcept {
    return (b - a).CalcLength4D();
}

float CalcDistance3D(const Vector4& a, const Vector4& b) noexcept {
    return (b - a).CalcLength3D();
}

float CalcDistanceSquared(const Vector2& a, const Vector2& b) noexcept {
    return (b - a).CalcLengthSquared();
}

float CalcDistanceSquared(const Vector3& a, const Vector3& b) noexcept {
    return (b - a).CalcLengthSquared();
}

float CalcDistanceSquared(const Vector4& a, const Vector4& b) noexcept {
    return (b - a).CalcLength4DSquared();
}

float CalcDistanceSquared(const Vector2& p, const LineSegment2& line) noexcept {
    return CalcDistanceSquared(p, CalcClosestPoint(p, line));
}

float CalcDistanceSquared(const Vector3& p, const LineSegment3& line) noexcept {
    return CalcDistanceSquared(p, CalcClosestPoint(p, line));
}

float CalcDistanceSquared4D(const Vector4& a, const Vector4& b) noexcept {
    return CalcDistanceSquared(a, b);
}

float CalcDistanceSquared3D(const Vector4& a, const Vector4& b) noexcept {
    return (b - a).CalcLength3DSquared();
}

float DotProduct(const Vector2& a, const Vector2& b) noexcept {
    return a.x * b.x + a.y * b.y;
}

float DotProduct(const Vector3& a, const Vector3& b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float DotProduct(const Vector4& a, const Vector4& b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float DotProduct(const Quaternion& a, const Quaternion& b) noexcept {
    return (a.w * b.w) + DotProduct(a.axis, b.axis);
}

Vector3 CrossProduct(const Vector3& a, const Vector3& b) noexcept {
    float a1 = a.x;
    float a2 = a.y;
    float a3 = a.z;

    float b1 = b.x;
    float b2 = b.y;
    float b3 = b.z;

    return Vector3(a2 * b3 - a3 * b2, a3 * b1 - a1 * b3, a1 * b2 - a2 * b1);
}

Vector2 Project(const Vector2& a, const Vector2& b) noexcept {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector3 Project(const Vector3& a, const Vector3& b) noexcept {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector4 Project(const Vector4& a, const Vector4& b) noexcept {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector2 Reflect(const Vector2& in, const Vector2& normal) noexcept {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector3 Reflect(const Vector3& in, const Vector3& normal) noexcept {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector4 Reflect(const Vector4& in, const Vector4& normal) noexcept {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector2 Rotate(const Vector2& v, const Quaternion& q) noexcept {
    return Vector2(Rotate(Vector3(v, 0.0f), q));
}

Vector3 Rotate(const Vector3& v, const Quaternion& q) noexcept {
    return (q * v * q.CalcInverse()).axis;
}

Vector2 ProjectAlongPlane(const Vector2& v, const Vector2& n) noexcept {
    return v - (DotProduct(v, n) * n);
}

Vector3 ProjectAlongPlane(const Vector3& v, const Vector3& n) noexcept {
    return v - (DotProduct(v, n) * n);
}

Vector4 ProjectAlongPlane(const Vector4& v, const Vector4& n) noexcept {
    return v - (DotProduct(v, n) * n);
}

unsigned int CalculateManhattanDistance(const IntVector2& start, const IntVector2& end) noexcept {
    return std::abs(end.x - start.x) + std::abs(end.y - start.y);
}

unsigned int CalculateManhattanDistance(const IntVector3& start, const IntVector3& end) noexcept {
    return std::abs(end.x - start.x) + std::abs(end.y - start.y) + std::abs(end.z - start.z);
}

unsigned int CalculateManhattanDistance(const IntVector4& start, const IntVector4& end) noexcept {
    return std::abs(end.x - start.x) + std::abs(end.y - start.y) + std::abs(end.z - start.z) + std::abs(end.w - start.w);
}

Vector2 GetRandomPointOn(const AABB2& aabb) noexcept {
    float result[2]{ 0.0f, 0.0f };
    int s = MathUtils::GetRandomIntLessThan(4);
    int c = s % 2;
    result[(c + 0) % 2] = s > 1 ? 1.0f : 0.0f;
    result[(c + 1) % 2] = s > 1 ? 1.0f : 0.0f;
    Vector2 point{ result[0], result[1]};
    return aabb.CalcCenter() + (point * aabb.CalcDimensions());
}

Vector2 GetRandomPointOn(const Disc2& disc) noexcept {
    Vector2 point{};
    point.SetLengthAndHeadingDegrees(MathUtils::GetRandomFloatZeroToOne() * 360.0f, disc.radius);
    return disc.center + point;
}

Vector2 GetRandomPointOn(const LineSegment2& line) noexcept {
    auto dir = line.CalcDirection();
    auto len = line.CalcLength() * MathUtils::GetRandomFloatZeroToOne();
    return line.start + (dir * len);
}

Vector3 GetRandomPointOn(const AABB3& aabb) noexcept {
    float result[3]{ 0.0f, 0.0f, 0.0f };
    int s = MathUtils::GetRandomIntLessThan(6);
    int c = s % 3;
    result[(c + 0) % 3] = s > 2 ? 1.0f : 0.0f;
    result[(c + 1) % 3] = MathUtils::GetRandomFloatZeroToOne();
    result[(c + 2) % 3] = MathUtils::GetRandomFloatZeroToOne();
    Vector3 point{ result[0], result[1], result[2] };
    return aabb.CalcCenter() + (point * aabb.CalcDimensions());
}

Vector3 GetRandomPointOn(const Sphere3& sphere) noexcept {
    //See: https://karthikkaranth.me/blog/generating-random-points-in-a-sphere/
    float u = MathUtils::GetRandomFloatZeroToOne();
    float v = MathUtils::GetRandomFloatZeroToOne();
    float theta = MathUtils::M_2PI * u;
    float phi = std::acos(2.0f * v - 1.0f);
    float r = sphere.radius;
    float sin_theta = std::sin(theta);
    float cos_theta = std::cos(theta);
    float sin_phi = std::sin(phi);
    float cos_phi = std::cos(phi);
    float x = r * sin_phi * cos_theta;
    float y = r * sin_phi * sin_theta;
    float z = r * cos_phi;
    return sphere.center + Vector3{x,y,z};
}

Vector3 GetRandomPointOn(const LineSegment3& line) noexcept {
    auto dir = line.CalcDirection();
    auto len = line.CalcLength() * MathUtils::GetRandomFloatZeroToOne();
    return line.start + (dir * len);
}

Vector2 GetRandomPointInside(const AABB2& aabb) noexcept {
    return Vector2{ MathUtils::GetRandomFloatInRange(aabb.mins.x, aabb.maxs.x), MathUtils::GetRandomFloatInRange(aabb.mins.y, aabb.maxs.y)};
}

Vector2 GetRandomPointInside(const Disc2& disc) noexcept {
    Vector2 point{};
    point.SetLengthAndHeadingDegrees(MathUtils::GetRandomFloatZeroToOne() * 360.0f, std::sqrt(MathUtils::GetRandomFloatZeroToOne()) * disc.radius);
    return disc.center + point;
}

Vector3 GetRandomPointInside(const AABB3& aabb) noexcept {
    return Vector3{ MathUtils::GetRandomFloatInRange(aabb.mins.x, aabb.maxs.x), MathUtils::GetRandomFloatInRange(aabb.mins.y, aabb.maxs.y),MathUtils::GetRandomFloatInRange(aabb.mins.z, aabb.maxs.z) };
}

Vector3 GetRandomPointInside(const Sphere3& sphere) noexcept {
    //See: https://karthikkaranth.me/blog/generating-random-points-in-a-sphere/
    float u = MathUtils::GetRandomFloatZeroToOne();
    float v = MathUtils::GetRandomFloatZeroToOne();
    float theta = MathUtils::M_2PI * u;
    float phi = std::acos(2.0f * v - 1.0f);
    float r = sphere.radius * std::pow(MathUtils::GetRandomFloatZeroToOne(), 1.0f / 3.0f);
    float sin_theta = std::sin(theta);
    float cos_theta = std::cos(theta);
    float sin_phi = std::sin(phi);
    float cos_phi = std::cos(phi);
    float x = r * sin_phi * cos_theta;
    float y = r * sin_phi * sin_theta;
    float z = r * cos_phi;
    return sphere.center + Vector3{ x,y,z };
}

bool IsPointInside(const AABB2& aabb, const Vector2& point) noexcept {
    if(aabb.maxs.x < point.x) return false;
    if(point.x < aabb.mins.x) return false;
    if(aabb.maxs.y < point.y) return false;
    if(point.y < aabb.mins.y) return false;
    return true;
}

bool IsPointInside(const AABB3& aabb, const Vector3& point) noexcept {
    if(aabb.maxs.x < point.x) return false;
    if(point.x < aabb.mins.x) return false;
    if(aabb.maxs.y < point.y) return false;
    if(point.y < aabb.mins.y) return false;
    if(aabb.maxs.z < point.z) return false;
    if(point.z < aabb.mins.z) return false;
    return true;
}

bool IsPointInside(const OBB2& /*obb*/, const Vector2& /*point*/) noexcept {
    return false;
}

bool IsPointInside(const Disc2& disc, const Vector2& point) noexcept {
    return CalcDistanceSquared(disc.center, point) < (disc.radius * disc.radius);
}

bool IsPointInside(const Capsule2& capsule, const Vector2& point) noexcept {
    return CalcDistanceSquared(point, capsule.line) < (capsule.radius * capsule.radius);
}

bool IsPointInside(const Sphere3& sphere, const Vector3& point) noexcept {
    return CalcDistanceSquared(sphere.center, point) < (sphere.radius * sphere.radius);
}

bool IsPointInside(const Capsule3& capsule, const Vector3& point) noexcept {
    return CalcDistanceSquared(point, capsule.line) < (capsule.radius * capsule.radius);
}

bool IsPointOn(const Disc2& disc, const Vector2& point) noexcept {
    float distanceSquared = CalcDistanceSquared(disc.center, point);
    float radiusSquared = disc.radius * disc.radius;
    return !(distanceSquared < radiusSquared || radiusSquared < distanceSquared);
}

bool IsPointOn(const LineSegment2& line, const Vector2& point) noexcept {
    return MathUtils::IsEquivalent(CalcDistanceSquared(point, line), 0.0f);
}

bool IsPointOn(const Capsule2& capsule, const Vector2& point) noexcept {
    float distanceSquared = CalcDistanceSquared(point, capsule.line);
    float radiusSquared = capsule.radius * capsule.radius;
    return !(distanceSquared < radiusSquared || radiusSquared < distanceSquared);
}

bool IsPointOn(const LineSegment3& line, const Vector3& point) noexcept {
    return MathUtils::IsEquivalent(CalcDistanceSquared(point, line), 0.0f);
}

bool IsPointOn(const Sphere3& sphere, const Vector3& point) noexcept {
    float distanceSquared = CalcDistanceSquared(sphere.center, point);
    float radiusSquared = sphere.radius * sphere.radius;
    return !(distanceSquared < radiusSquared || radiusSquared < distanceSquared);
}

bool IsPointOn(const Capsule3& capsule, const Vector3& point) noexcept {
    float distanceSquared = CalcDistanceSquared(point, capsule.line);
    float radiusSquared = capsule.radius * capsule.radius;
    return !(distanceSquared < radiusSquared || radiusSquared < distanceSquared);
}

Vector2 CalcClosestPoint(const Vector2& p, const AABB2& aabb) noexcept {
    if(IsPointInside(aabb, p)) {
        return p;
    }
    if(p.x < aabb.mins.x && aabb.maxs.y < p.y) {
        return Vector2(aabb.mins.x, aabb.maxs.y);
    }
    if(p.x < aabb.mins.x && p.y < aabb.mins.y) {
        return Vector2(aabb.mins.x, aabb.mins.y);
    }
    if(aabb.maxs.x < p.x && p.y < aabb.mins.y) {
        return Vector2(aabb.maxs.x, aabb.mins.y);
    }
    if(aabb.maxs.x < p.x && aabb.maxs.y < p.y) {
        return Vector2(aabb.maxs.x, aabb.maxs.y);
    }
    if(p.x < aabb.mins.x) {
        return Vector2(aabb.mins.x, p.y);
    }
    if(aabb.maxs.x < p.x) {
        return Vector2(aabb.maxs.x, p.y);
    }
    if(p.y < aabb.mins.y) {
        return Vector2(p.x, aabb.mins.y);
    }
    if(aabb.maxs.y < p.y) {
        return Vector2(p.x, aabb.maxs.y);
    }
    return Vector2::ZERO;
}

Vector3 CalcClosestPoint(const Vector3& p, const AABB3& aabb) noexcept {
    float nearestX = std::clamp(p.x, aabb.mins.x, aabb.maxs.x);
    float nearestY = std::clamp(p.y, aabb.mins.y, aabb.maxs.y);
    float nearestZ = std::clamp(p.z, aabb.mins.z, aabb.maxs.z);

    return Vector3(nearestX, nearestY, nearestZ);
}

Vector2 CalcClosestPoint(const Vector2& p, const Disc2& disc) noexcept {
    Vector2 dir = (p - disc.center).GetNormalize();
    return disc.center + dir * disc.radius;
}

Vector2 CalcClosestPoint(const Vector2& p, const LineSegment2& line) noexcept {
    Vector2 D = line.end - line.start;
    Vector2 T = D.GetNormalize();

    Vector2 SP = p - line.start;
    float regionI = MathUtils::DotProduct(T, SP);
    if(regionI < 0.0f) {
        return line.start;
    }
    
    Vector2 EP = p - line.end;
    float regionII = MathUtils::DotProduct(T, EP);
    if(regionII > 0.0f) {
        return line.end;
    }

    Vector2 directionSE = D.GetNormalize();
    float lengthToClosestPoint = MathUtils::DotProduct(directionSE, SP);
    Vector2 C = directionSE * lengthToClosestPoint;
    Vector2 ConL = line.start + C;
    return ConL;
}

Vector2 CalcClosestPoint(const Vector2& p, const Capsule2& capsule) noexcept {
    Vector2 closestP = CalcClosestPoint(p, capsule.line);
    Vector2 dir_to_p = (p - closestP).GetNormalize();
    return closestP + (dir_to_p * capsule.radius);
}

Vector3 CalcClosestPoint(const Vector3& p, const LineSegment3& line) noexcept {
    Vector3 D = line.end - line.start;
    Vector3 T = D.GetNormalize();
    Vector3 SP = p - line.start;
    float regionI = MathUtils::DotProduct(T, SP);
    if(regionI < 0.0f) {
        return line.start;
    }

    Vector3 EP = p - line.end;
    float regionII = MathUtils::DotProduct(T, EP);
    if(regionII > 0.0f) {
        return line.end;
    }

    Vector3 directionSE = D.GetNormalize();
    float lengthToClosestPoint = MathUtils::DotProduct(directionSE, SP);
    Vector3 C = directionSE * lengthToClosestPoint;
    Vector3 ConL = line.start + C;
    return ConL;
}

Vector3 CalcClosestPoint(const Vector3& p, const Sphere3& sphere) noexcept {
    Vector3 dir = (p - sphere.center).GetNormalize();
    return sphere.center + dir * sphere.radius;
}

Vector3 CalcClosestPoint(const Vector3& p, const Capsule3& capsule) noexcept {
    Vector3 closestP = CalcClosestPoint(p, capsule.line);
    Vector3 dir_to_p = (p - closestP).GetNormalize();
    return closestP + (dir_to_p * capsule.radius);
}

Vector2 CalcNormalizedPointFromPoint(const Vector2& pos, const AABB2& bounds) noexcept {
    float x_norm = RangeMap(pos.x, bounds.mins.x, bounds.maxs.x, 0.0f, 1.0f);
    float y_norm = RangeMap(pos.y, bounds.mins.y, bounds.maxs.y, 0.0f, 1.0f);
    return Vector2(x_norm, y_norm);
}

Vector2 CalcPointFromNormalizedPoint(const Vector2& uv, const AABB2& bounds) noexcept {
    float x = RangeMap(uv.x, 0.0f, 1.0f, bounds.mins.x, bounds.maxs.x);
    float y = RangeMap(uv.y, 0.0f, 1.0f, bounds.mins.y, bounds.maxs.y);
    return Vector2(x, y);
}

Vector2 CalcNormalizedHalfExtentsFromPoint(const Vector2& pos, const AABB2& bounds) noexcept {
    float x_norm = RangeMap(pos.x, bounds.mins.x, bounds.maxs.x, -0.5f, 0.5f);
    float y_norm = RangeMap(pos.y, bounds.mins.y, bounds.maxs.y, -0.5f, 0.5f);
    return Vector2(x_norm, y_norm);
}

Vector2 CalcPointFromNormalizedHalfExtents(const Vector2& uv, const AABB2& bounds) noexcept {
    float x = RangeMap(uv.x, -0.5f, 0.5f, bounds.mins.x, bounds.maxs.x);
    float y = RangeMap(uv.y, -0.5f, 0.5f, bounds.mins.y, bounds.maxs.y);
    return Vector2(x, y);
}

bool DoDiscsOverlap(const Disc2& a, const Disc2& b) noexcept {
    return DoDiscsOverlap(a.center, a.radius, b.center, b.radius);
}

bool DoDiscsOverlap(const Vector2& centerA, float radiusA, const Vector2& centerB, float radiusB) noexcept {
    return CalcDistanceSquared(centerA, centerB) < (radiusA + radiusB) * (radiusA + radiusB);
}

bool DoDiscsOverlap(const Disc2& a, const Capsule2& b) noexcept {
    return CalcDistanceSquared(a.center, b.line) < (a.radius + b.radius) * (a.radius + b.radius);
}

bool DoSpheresOverlap(const Sphere3& a, const Sphere3& b) noexcept {
    return DoSpheresOverlap(a.center, a.radius, b.center, b.radius);
}

bool DoSpheresOverlap(const Vector3& centerA, float radiusA, const Vector3& centerB, float radiusB) noexcept {
    return CalcDistanceSquared(centerA, centerB) < (radiusA + radiusB) * (radiusA + radiusB);
}

bool DoSpheresOverlap(const Sphere3& a, const Capsule3& b) noexcept {
    return CalcDistanceSquared(a.center, b.line) < (a.radius + b.radius) * (a.radius + b.radius);
}

bool DoAABBsOverlap(const AABB2& a, const AABB2& b) noexcept {
    if(a.maxs.x < b.mins.x) return false;
    if(b.maxs.x < a.mins.x) return false;
    if(a.maxs.y < b.mins.y) return false;
    if(b.maxs.y < a.mins.y) return false;
    return true;
}

bool DoAABBsOverlap(const AABB3& a, const AABB3& b) noexcept {
    if(a.maxs.x < b.mins.x) return false;
    if(b.maxs.x < a.mins.x) return false;
    if(a.maxs.y < b.mins.y) return false;
    if(b.maxs.y < a.mins.y) return false;
    if(a.maxs.z < b.mins.z) return false;
    if(b.maxs.z < a.mins.z) return false;
    return true;
}

bool DoOBBsOverlap(const OBB2& a, const OBB2& b) noexcept {
    //Separating Axis Theorem
    const auto Pa = a.position;
    const auto Oa = a.orientationDegrees;
    const auto Ra = Matrix4::Create2DRotationDegreesMatrix(Oa);
    const auto Ta = Matrix4::CreateTranslationMatrix(Pa);
    const auto Ma = Ta * Ra;
    const auto a_hex = a.half_extents.x;
    const auto a_hey = a.half_extents.y;
    const auto a_topright = Ma.TransformPosition(Vector2(+a_hex,-a_hey));
    const auto a_bottomright = Ma.TransformPosition(Vector2(+a_hex,+a_hey));
    const auto a_topleft = Ma.TransformPosition(Vector2(-a_hex,-a_hey));
    const auto a_bottomleft = Ma.TransformPosition(Vector2(-a_hex,+a_hey));
    const auto a_right_normal = Ra.TransformDirection(Vector2(a_hex, 0.0f).GetNormalize());
    const auto a_down_normal = Ra.TransformDirection(Vector2(0.0f, a_hey).GetNormalize());
    const auto a_left_normal = -a_right_normal;
    const auto a_up_normal = -a_down_normal;

    const auto Pb = b.position;
    const auto Ob = b.orientationDegrees;
    const auto Rb = Matrix4::Create2DRotationDegreesMatrix(Ob);
    const auto Tb = Matrix4::CreateTranslationMatrix(Pb);
    const auto Mb = Tb * Rb;
    const auto b_hex = b.half_extents.x;
    const auto b_hey = b.half_extents.y;
    const auto b_topright = Mb.TransformPosition(Vector2(+b_hex, -b_hey));
    const auto b_bottomright = Mb.TransformPosition(Vector2(+b_hex, +b_hey));
    const auto b_topleft = Mb.TransformPosition(Vector2(-b_hex, -b_hey));
    const auto b_bottomleft = Mb.TransformPosition(Vector2(-b_hex, +b_hey));
    const auto b_right_normal = Rb.TransformDirection(Vector2(b_hex, 0.0f).GetNormalize());
    const auto b_down_normal = Rb.TransformDirection(Vector2(0.0f, b_hey).GetNormalize());
    const auto b_left_normal = -b_right_normal;
    const auto b_up_normal = -b_down_normal;

    const std::vector<Vector2> a_normals{ a_right_normal, a_down_normal};
    const std::vector<Vector2> a_corners{ a_bottomleft, a_topleft, a_topright, a_bottomright};
    const std::vector<Vector2> b_normals{ b_right_normal, b_down_normal};
    const std::vector<Vector2> b_corners{ b_bottomleft, b_topleft, b_topright, b_bottomright};

    for(const auto& an : a_normals) {
        auto min_a = std::numeric_limits<float>::infinity();
        auto max_a = std::numeric_limits<float>::lowest();
        for(const auto& ac : a_corners) {
            const auto proj_dp = DotProduct(ac, an);
            min_a = (std::min)(min_a, proj_dp);
            max_a = (std::max)(max_a, proj_dp);
        }

        auto min_b = std::numeric_limits<float>::infinity();
        auto max_b = std::numeric_limits<float>::lowest();
        for(const auto& bc : b_corners) {
            const auto proj_dp = DotProduct(bc, an);
            min_b = (std::min)(min_b, proj_dp);
            max_b = (std::max)(max_b, proj_dp);
        }

        if(max_a < min_b) return false;
        if(max_b < min_a) return false;
    }
    for(const auto& bn : b_normals) {
        auto min_b = std::numeric_limits<float>::infinity();
        auto max_b = std::numeric_limits<float>::lowest();
        for(const auto& bc : b_corners) {
            const auto proj_dp = DotProduct(bc, bn);
            min_b = (std::min)(min_b, proj_dp);
            max_b = (std::max)(max_b, proj_dp);
        }

        auto min_a = std::numeric_limits<float>::infinity();
        auto max_a = std::numeric_limits<float>::lowest();
        for(const auto& ac : a_corners) {
            const auto proj_dp = DotProduct(ac, bn);
            min_a = (std::min)(min_a, proj_dp);
            max_a = (std::max)(max_a, proj_dp);
        }

        if(max_b < min_a) return false;
        if(max_a < min_b) return false;
    }
    return true;
}

bool DoLineSegmentOverlap(const Disc2& a, const LineSegment2& b) noexcept {
    return CalcDistanceSquared(a.center, b) < a.radius * a.radius;
}

bool DoLineSegmentOverlap(const Sphere3& a, const LineSegment3& b) noexcept {
    return CalcDistanceSquared(a.center, b) < a.radius * a.radius;
}

bool DoCapsuleOverlap(const Disc2& a, const Capsule2& b) noexcept {
    return CalcDistanceSquared(a.center, b.line) < (a.radius + b.radius) * (a.radius + b.radius);
}

bool DoCapsuleOverlap(const Sphere3& a, const Capsule3& b) noexcept {
    return CalcDistanceSquared(a.center, b.line) < (a.radius + b.radius) * (a.radius + b.radius);
}

bool DoPlaneOverlap(const Disc2& a, const Plane2& b) noexcept {
    return std::fabs(DotProduct(a.center, b.normal) - b.dist) < a.radius;
}

bool DoPlaneOverlap(const Sphere3& a, const Plane3& b) noexcept {
    return std::fabs(DotProduct(a.center, b.normal) - b.dist) < a.radius;
}

bool DoPlaneOverlap(const Capsule2& a, const Plane2& b) noexcept {
    bool both_capsule_points_in_front = IsPointInFrontOfPlane(a.line.start, b) && IsPointInFrontOfPlane(a.line.end, b);
    bool both_capsule_points_in_back = IsPointBehindOfPlane(a.line.start, b) && IsPointBehindOfPlane(a.line.end, b);

    if(both_capsule_points_in_front || both_capsule_points_in_back) {
        return CalcDistanceSquared(Vector2::ZERO, a.line) < (a.radius + b.dist) * (a.radius + b.dist);
    }
    return true;
}

bool DoPlaneOverlap(const Capsule3& a, const Plane3& b) noexcept {
    bool both_capsule_points_in_front = IsPointInFrontOfPlane(a.line.start, b) && IsPointInFrontOfPlane(a.line.end, b);
    bool both_capsule_points_in_back = IsPointBehindOfPlane(a.line.start, b) && IsPointBehindOfPlane(a.line.end, b);

    if(both_capsule_points_in_front || both_capsule_points_in_back) {
        return CalcDistanceSquared(Vector3::ZERO, a.line) < (a.radius + b.dist) * (a.radius + b.dist);
    }
    return true;
}

bool IsPointInFrontOfPlane(const Vector3& point, const Plane3& plane) noexcept {
    return (DotProduct(point, plane.normal) > plane.dist);
}

bool IsPointBehindOfPlane(const Vector3& point, const Plane3& plane) noexcept {
    return (DotProduct(point, plane.normal) < plane.dist);
}

bool IsPointOnPlane(const Vector3& point, const Plane3& plane) noexcept {
    return !IsPointInFrontOfPlane(point, plane) && !IsPointBehindOfPlane(point, plane);
}

bool IsPointInFrontOfPlane(const Vector2& point, const Plane2& plane) noexcept {
    return (DotProduct(point, plane.normal) > plane.dist);
}

bool IsPointBehindOfPlane(const Vector2& point, const Plane2& plane) noexcept {
    return (DotProduct(point, plane.normal) < plane.dist);
}

bool IsPointOnPlane(const Vector2& point, const Plane2& plane) noexcept {
    return !IsPointInFrontOfPlane(point, plane) && !IsPointBehindOfPlane(point, plane);
}

float CalculateMatrix3Determinant(float m00, float m01, float m02,
                                  float m10, float m11, float m12,
                                  float m20, float m21, float m22) noexcept {
    float a = m00;
    float b = m01;
    float c = m02;
    float det_not_a = CalculateMatrix2Determinant(m11, m12, m21, m22);
    float det_not_b = CalculateMatrix2Determinant(m10, m12, m20, m22);
    float det_not_c = CalculateMatrix2Determinant(m10, m11, m20, m21);

    return a * det_not_a - b * det_not_b + c * det_not_c;
}

float CalculateMatrix2Determinant(float m00, float m01,
                                  float m10, float m11) noexcept {
    return m00 * m11 - m01 * m10;
}

/************************************************************************/
/* https://en.wikipedia.org/wiki/Slerp#Source_Code                      */
/************************************************************************/
Quaternion SLERP(const Quaternion& a, const Quaternion& b, float t) noexcept {
    Quaternion start = a;
    Quaternion end = b;

    start.Normalize();
    end.Normalize();

    float dp = MathUtils::DotProduct(start, end);

    if(dp < 0.0f) {
        end = -end;
        dp = -dp;
    }

    //Really close together
    if(dp > 0.99995f) {
        Quaternion result = MathUtils::Interpolate(start, end, t);
        result.Normalize();
        return result;
    }

    dp = std::clamp(dp, -1.0f, 1.0f);

    float theta_0 = std::acos(dp);
    float theta = theta_0 * t;

    float scale0 = std::cos(theta) - dp * std::sin(theta) / std::sin(theta_0);
    float scale1 = std::sin(theta) / std::sin(theta_0);

    return (scale0 * start) + (scale1 * end);
}

template<>
Vector2 Interpolate(const Vector2& a, const Vector2& b, float t) {
    float x = Interpolate(a.x, b.x, t);
    float y = Interpolate(a.y, b.y, t);
    return Vector2(x, y);
}

template<>
Vector3 Interpolate(const Vector3& a, const Vector3& b, float t) {
    float x = Interpolate(a.x, b.x, t);
    float y = Interpolate(a.y, b.y, t);
    float z = Interpolate(a.z, b.z, t);
    return Vector3(x, y, z);
}

template<>
Vector4 Interpolate(const Vector4& a, const Vector4& b, float t) {
    float x = Interpolate(a.x, b.x, t);
    float y = Interpolate(a.y, b.y, t);
    float z = Interpolate(a.z, b.z, t);
    float w = Interpolate(a.w, b.w, t);
    return Vector4(x, y, z, w);
}

template<>
IntVector2 Interpolate(const IntVector2& a, const IntVector2& b, float t) {
    float x = Interpolate(static_cast<float>(a.x), static_cast<float>(b.x), t);
    float y = Interpolate(static_cast<float>(a.y), static_cast<float>(b.y), t);
    return IntVector2(Vector2(x, y));
}

template<>
IntVector3 Interpolate(const IntVector3& a, const IntVector3& b, float t) {
    float x = Interpolate(static_cast<float>(a.x), static_cast<float>(b.x), t);
    float y = Interpolate(static_cast<float>(a.y), static_cast<float>(b.y), t);
    float z = Interpolate(static_cast<float>(a.z), static_cast<float>(b.z), t);
    return IntVector3(Vector3(x, y, z));
}

template<>
IntVector4 Interpolate(const IntVector4& a, const IntVector4& b, float t) {
    float x = Interpolate(static_cast<float>(a.x), static_cast<float>(b.x), t);
    float y = Interpolate(static_cast<float>(a.y), static_cast<float>(b.y), t);
    float z = Interpolate(static_cast<float>(a.z), static_cast<float>(b.z), t);
    float w = Interpolate(static_cast<float>(a.w), static_cast<float>(b.w), t);
    return IntVector4(Vector4(x, y, z, w));
}

template<>
AABB2 Interpolate(const AABB2& a, const AABB2& b, float t) {
    Vector2 mins(Interpolate(a.mins, b.mins, t));
    Vector2 maxs(Interpolate(a.maxs, b.maxs, t));
    return AABB2(mins, maxs);
}

template<>
AABB3 Interpolate(const AABB3& a, const AABB3& b, float t) {
    Vector3 mins(Interpolate(a.mins, b.mins, t));
    Vector3 maxs(Interpolate(a.maxs, b.maxs, t));
    return AABB3(mins, maxs);
}

template<>
Disc2 Interpolate(const Disc2& a, const Disc2& b, float t) {
    Vector2 c(Interpolate(a.center, b.center, t));
    float r(Interpolate(a.radius, b.radius, t));
    return Disc2(c, r);
}

template<>
LineSegment2 Interpolate(const LineSegment2& a, const LineSegment2& b, float t) {
    Vector2 start(Interpolate(a.start, b.start, t));
    Vector2 end(Interpolate(a.end, b.end, t));
    return LineSegment2(start, end);
}

template<>
Capsule2 Interpolate(const Capsule2& a, const Capsule2& b, float t) {
    LineSegment2 line(Interpolate(a.line, b.line, t));
    float r(Interpolate(a.radius, b.radius, t));
    return Capsule2(line, r);
}

template<>
LineSegment3 Interpolate(const LineSegment3& a, const LineSegment3& b, float t) {
    Vector3 start(Interpolate(a.start, b.start, t));
    Vector3 end(Interpolate(a.end, b.end, t));
    return LineSegment3(start, end);
}

template<>
Sphere3 Interpolate(const Sphere3& a, const Sphere3& b, float t) {
    Vector3 c(Interpolate(a.center, b.center, t));
    float r(Interpolate(a.radius, b.radius, t));
    return Sphere3(c, r);
}

template<>
Capsule3 Interpolate(const Capsule3& a, const Capsule3& b, float t) {
    LineSegment3 line(Interpolate(a.line, b.line, t));
    float r(Interpolate(a.radius, b.radius, t));
    return Capsule3(line, r);
}

template<>
Plane2 Interpolate(const Plane2& a, const Plane2& b, float t) {
    float d = Interpolate(a.dist, b.dist, t);
    Vector2 n = Interpolate(a.normal, b.normal, t);
    return Plane2(n, d);
}

template<>
Plane3 Interpolate(const Plane3& a, const Plane3& b, float t) {
    float d = Interpolate(a.dist, b.dist, t);
    Vector3 n = Interpolate(a.normal, b.normal, t);
    return Plane3(n, d);
}

template<>
Quaternion Interpolate(const Quaternion& a, const Quaternion& b, float t) {
    float w = Interpolate(a.w, b.w, t);
    Vector3 axis = Interpolate(a.axis, b.axis, t);
    return Quaternion(w, axis);
}

template<>
Rgba Interpolate(const Rgba& a, const Rgba& b, float t) {
    float a_color[4];
    a.GetAsFloats(a_color[0], a_color[1], a_color[2], a_color[3]);
    float b_color[4];
    b.GetAsFloats(b_color[0], b_color[1], b_color[2], b_color[3]);

    float red   = Interpolate(a_color[0], b_color[0], t);
    float green = Interpolate(a_color[1], b_color[1], t);
    float blue  = Interpolate(a_color[2], b_color[2], t);
    float alpha = Interpolate(a_color[3], b_color[3], t);

    Rgba result;
    result.SetAsFloats(red, green, blue, alpha);
    return result;
}

Vector2 RangeMap(const Vector2& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange) {
    auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return Vector2{ x, y };
}

Vector3 RangeMap(const Vector3& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange) {
    auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto z = RangeMap(valueToMap.z, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return Vector3{ x, y, z };
}

Vector4 RangeMap(const Vector4& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange) {
    auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto z = RangeMap(valueToMap.z, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto w = RangeMap(valueToMap.w, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return Vector4{ x, y, z, w };
}

IntVector2 RangeMap(const IntVector2& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange) {
    auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return IntVector2{ x, y };
}

IntVector3 RangeMap(const IntVector3& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange) {
    auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto z = RangeMap(valueToMap.z, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return IntVector3{ x, y, z };
}

IntVector4 RangeMap(const IntVector4& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange) {
    auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto z = RangeMap(valueToMap.z, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    auto w = RangeMap(valueToMap.w, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return IntVector4{ x, y, z, w };
}

template<>
Vector4 Wrap(const Vector4& valuesToWrap, const Vector4& minValues, const Vector4& maxValues) {
    auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    auto z = Wrap(valuesToWrap.z, minValues.z, maxValues.z);
    auto w = Wrap(valuesToWrap.w, minValues.w, maxValues.w);
    return Vector4(x, y, z, w);
}

template<>
Vector3 Wrap(const Vector3& valuesToWrap, const Vector3& minValues, const Vector3& maxValues) {
    auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    auto z = Wrap(valuesToWrap.z, minValues.z, maxValues.z);
    return Vector3(x, y, z);
}

template<>
Vector2 Wrap(const Vector2& valuesToWrap, const Vector2& minValues, const Vector2& maxValues) {
    auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    return Vector2(x, y);
}

template<>
IntVector4 Wrap(const IntVector4& valuesToWrap, const IntVector4& minValues, const IntVector4& maxValues) {
    auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    auto z = Wrap(valuesToWrap.z, minValues.z, maxValues.z);
    auto w = Wrap(valuesToWrap.w, minValues.w, maxValues.w);
    return IntVector4(x, y, z, w);
}

template<>
IntVector3 Wrap(const IntVector3& valuesToWrap, const IntVector3& minValues, const IntVector3& maxValues) {
    auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    auto z = Wrap(valuesToWrap.z, minValues.z, maxValues.z);
    return IntVector3(x, y, z);
}

template<>
IntVector2 Wrap(const IntVector2& valuesToWrap, const IntVector2& minValues, const IntVector2& maxValues) {
    auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    return IntVector2(x, y);
}

} //End MathUtils