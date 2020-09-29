#pragma once

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

#include <algorithm>
#include <optional>
#include <random>
#include <ratio>
#include <utility>

class AABB2;
class AABB3;
class Capsule2;
class Disc2;
class LineSegment2;
class LineSegment3;
class OBB2;
class Sphere3;
class Capsule3;
class Plane2;
class Plane3;
class Polygon2;
class Quaternion;
class Rgba;

namespace MathUtils {
//TODO: Consider <numbers> header when it becomes available
constexpr const float M_1PI_6 = 0.52359877559829887307f;         // 1pi/6
constexpr const float M_1PI_4 = 0.78539816339744830962f;         // 1pi/4
constexpr const float M_1PI_3 = 1.04719755119659774615f;         // 1pi/3
constexpr const float M_1PI_2 = 1.57079632679489661923f;         // 1pi/2
constexpr const float M_2PI_3 = 2.09439510239319549230f;         // 2pi/3
constexpr const float M_3PI_4 = 2.35619449019234492884f;         // 3pi/4
constexpr const float M_5PI_6 = 2.61799387799149436538f;         // 5pi/6
constexpr const float M_PI = 3.14159265358979323846f;            // pi
constexpr const float M_7PI_6 = 3.66519142918809211153f;         // 7pi/6
constexpr const float M_5PI_4 = 3.92699081698724154807f;         // 5pi/4
constexpr const float M_4PI_3 = 4.18879020478639098461f;         // 4pi/3
constexpr const float M_3PI_2 = 4.71238898038468985769f;         // 3pi/2
constexpr const float M_5PI_3 = 5.23598775598298873077f;         // 5pi/3
constexpr const float M_7PI_4 = 5.49778714378213816730f;         // 7pi/4
constexpr const float M_11PI_6 = 5.75958653158128760384f;        // 11pi/6
constexpr const float M_2PI = 6.28318530717958647692f;           // 2pi
constexpr const float M_E = 2.71828182845904523536f;             // e
constexpr const float M_LOG2E = 1.44269504088896340736f;         // log2(e)
constexpr const float M_LOG10E = 0.43429448190325182765f;        // log10(e)
constexpr const float M_LN2 = 0.69314718055994530942f;           // ln(2)
constexpr const float M_LN10 = 2.30258509299404568402f;          // ln(10)
constexpr const float M_PI_2 = 1.57079632679489661923f;          // pi/2
constexpr const float M_PI_4 = 0.78539816339744830962f;          // pi/4
constexpr const float M_1_PI = 0.31830988618379067151f;          // 1/pi
constexpr const float M_2_PI = 0.63661977236758134308f;          // 2/pi
constexpr const float M_2_SQRTPI = 1.12837916709551257390f;      // 2/sqrt(pi)
constexpr const float M_SQRT2 = 1.41421356237309504880f;         // sqrt(2)
constexpr const float M_1_SQRT2 = 0.70710678118654752440f;       // 1/sqrt(2)
constexpr const float M_SQRT3 = 1.73205080756887729352f;         // sqrt(3)
constexpr const float M_1_SQRT3 = 0.57735026918962576450f;       // 1/sqrt(3)
constexpr const float M_SQRT3_3 = 0.57735026918962576451f;       // sqrt(3)/3
constexpr const float M_TAU = 1.61803398874989484821f;           // tau (golden ratio)
constexpr const float M_16_BY_9_RATIO = 1.77777777777777777778f; // 16/9
constexpr const float M_4_BY_3_RATIO = 1.33333333333333333333f;  // 4/3
constexpr const std::ratio<1, 1024> KIB_BYTES_RATIO;             // Kilobyte/Bytes
constexpr const std::ratio<1, 1048576> MIB_BYTES_RATIO;          // Megabyte/Bytes
constexpr const std::ratio<1, 1073741824> GIB_BYTES_RATIO;       // Gigabyte/Bytes
constexpr const std::ratio<1024, 1> BYTES_KIB_RATIO;             // Bytes/Kilobytes
constexpr const std::ratio<1048576, 1> BYTES_MIB_RATIO;          // Bytes/Megabytes
constexpr const std::ratio<1073741824, 1> BYTES_GIB_RATIO;       // Bytes/Gigabytes

void SetRandomEngineSeed(unsigned int seed) noexcept;
std::random_device& GetRandomDevice() noexcept;
std::mt19937& GetMTRandomEngine(unsigned int seed = 0) noexcept;
std::mt19937_64& GetMT64RandomEngine(unsigned int seed = 0) noexcept;

std::pair<float, float> SplitFloatingPointValue(float value) noexcept;
std::pair<double, double> SplitFloatingPointValue(double value) noexcept;
std::pair<long double, long double> SplitFloatingPointValue(long double value) noexcept;

float ConvertDegreesToRadians(float degrees) noexcept;
float ConvertRadiansToDegrees(float radians) noexcept;

bool GetRandomBool() noexcept;

int GetRandomIntLessThan(int maxValueNotInclusive) noexcept;
int GetRandomIntInRange(int minInclusive, int maxInclusive) noexcept;

long GetRandomLongLessThan(long maxValueNotInclusive) noexcept;
long GetRandomLongInRange(long minInclusive, long maxInclusive) noexcept;

long long GetRandomLongLongLessThan(long long maxValueNotInclusive) noexcept;
long long GetRandomLongLongInRange(long long minInclusive, long long maxInclusive) noexcept;

float GetRandomFloatInRange(float minInclusive, float maxInclusive) noexcept;
float GetRandomFloatZeroToOne() noexcept;
float GetRandomFloatZeroUpToOne() noexcept;
float GetRandomFloatNegOneToOne() noexcept;
bool IsPercentChance(float probability) noexcept;

double GetRandomDoubleInRange(double minInclusive, double maxInclusive) noexcept;
double GetRandomDoubleZeroToOne() noexcept;
double GetRandomDoubleZeroUpToOne() noexcept;
double GetRandomDoubleNegOneToOne() noexcept;
bool IsPercentChance(double probability) noexcept;

long double GetRandomLongDoubleInRange(long double minInclusive, long double maxInclusive) noexcept;
long double GetRandomLongDoubleZeroToOne() noexcept;
long double GetRandomLongDoubleZeroUpToOne() noexcept;
long double GetRandomLongDoubleNegOneToOne() noexcept;
bool IsPercentChance(long double probability) noexcept;

float CosDegrees(float degrees) noexcept;
float SinDegrees(float degrees) noexcept;
float Atan2Degrees(float y, float x) noexcept;

bool IsEquivalent(float a, float b, float epsilon = 0.00001f) noexcept;
bool IsEquivalent(double a, double b, double epsilon = 0.0001) noexcept;
bool IsEquivalent(long double a, long double b, long double epsilon = 0.0001L) noexcept;
bool IsEquivalent(const Vector2& a, const Vector2& b, float epsilon = 0.0001f) noexcept;
bool IsEquivalent(const Vector3& a, const Vector3& b, float epsilon = 0.0001f) noexcept;
bool IsEquivalent(const Vector4& a, const Vector4& b, float epsilon = 0.0001f) noexcept;
bool IsEquivalent(const Quaternion& a, const Quaternion& b, float epsilon = 0.0001f) noexcept;

bool IsEquivalentOrLessThan(float a, float b, float epsilon = 0.00001f) noexcept;
bool IsEquivalentOrLessThan(double a, double b, double epsilon = 0.0001) noexcept;
bool IsEquivalentOrLessThan(long double a, long double b, long double epsilon = 0.0001L) noexcept;

bool IsEquivalentOrLessThanZero(float a, float epsilon = 0.00001f) noexcept;
bool IsEquivalentOrLessThanZero(double a, double epsilon = 0.0001) noexcept;
bool IsEquivalentOrLessThanZero(long double a, long double epsilon = 0.0001L) noexcept;

bool IsEquivalentToZero(float a, float epsilon = 0.00001f) noexcept;
bool IsEquivalentToZero(double a, double epsilon = 0.0001) noexcept;
bool IsEquivalentToZero(long double a, long double epsilon = 0.0001L) noexcept;
bool IsEquivalentToZero(const Vector2& a, float epsilon = 0.0001f) noexcept;
bool IsEquivalentToZero(const Vector3& a, float epsilon = 0.0001f) noexcept;
bool IsEquivalentToZero(const Vector4& a, float epsilon = 0.0001f) noexcept;
bool IsEquivalentToZero(const Quaternion& a, float epsilon = 0.0001f) noexcept;

float CalcDistance(const Vector2& a, const Vector2& b) noexcept;
float CalcDistance(const Vector3& a, const Vector3& b) noexcept;
float CalcDistance(const Vector4& a, const Vector4& b) noexcept;
float CalcDistance(const Vector2& p, const LineSegment2& line) noexcept;
float CalcDistance(const Vector3& p, const LineSegment3& line) noexcept;
float CalcDistance(const Vector2& p, const Polygon2& poly2) noexcept;
float CalcDistance(const LineSegment2& line, const Polygon2& poly2) noexcept;
float CalcDistance(const Polygon2& poly2, const LineSegment2& line) noexcept;
float CalcDistance(const LineSegment2& lineA, const LineSegment2& lineB) noexcept;

float CalcDistanceSquared(const Vector2& a, const Vector2& b) noexcept;
float CalcDistanceSquared(const Vector3& a, const Vector3& b) noexcept;
float CalcDistanceSquared(const Vector4& a, const Vector4& b) noexcept;
float CalcDistanceSquared(const Vector2& p, const LineSegment2& line) noexcept;
float CalcDistanceSquared(const Vector3& p, const LineSegment3& line) noexcept;
float CalcDistanceSquared(const Vector2& p, const Polygon2& poly2) noexcept;
float CalcDistanceSquared(const LineSegment2& line, const Polygon2& poly2) noexcept;
float CalcDistanceSquared(const Polygon2& poly2, const LineSegment2& line) noexcept;
float CalcDistanceSquared(const LineSegment2& lineA, const LineSegment2& lineB) noexcept;

float CrossProduct(const Vector2& a, const Vector2& b) noexcept;
Vector3 CrossProduct(const Vector3& a, const Vector3& b) noexcept;

float DotProduct(const Vector2& a, const Vector2& b) noexcept;
float DotProduct(const Vector3& a, const Vector3& b) noexcept;
float DotProduct(const Vector4& a, const Vector4& b) noexcept;
float DotProduct(const Quaternion& a, const Quaternion& b) noexcept;

float TripleProductScalar(const Vector3& a, const Vector3& b, const Vector3& c) noexcept;
Vector2 TripleProductVector(const Vector2& a, const Vector2& b, const Vector2& c) noexcept;
Vector3 TripleProductVector(const Vector3& a, const Vector3& b, const Vector3& c) noexcept;

Vector2 Project(const Vector2& a, const Vector2& b) noexcept;
Vector3 Project(const Vector3& a, const Vector3& b) noexcept;
Vector4 Project(const Vector4& a, const Vector4& b) noexcept;

Vector2 Reject(const Vector2& a, const Vector2& b) noexcept;
Vector3 Reject(const Vector3& a, const Vector3& b) noexcept;
Vector4 Reject(const Vector4& a, const Vector4& b) noexcept;

std::pair<Vector2, Vector2> DivideIntoProjectAndReject(const Vector2& a, const Vector2& b) noexcept;
std::pair<Vector3, Vector3> DivideIntoProjectAndReject(const Vector3& a, const Vector3& b) noexcept;
std::pair<Vector4, Vector4> DivideIntoProjectAndReject(const Vector4& a, const Vector4& b) noexcept;

Vector2 Reflect(const Vector2& in, const Vector2& normal) noexcept;
Vector3 Reflect(const Vector3& in, const Vector3& normal) noexcept;
Vector4 Reflect(const Vector4& in, const Vector4& normal) noexcept;

Vector2 Rotate(const Vector2& v, const Quaternion& q) noexcept;
Vector3 Rotate(const Vector3& v, const Quaternion& q) noexcept;

Vector2 ProjectAlongPlane(const Vector2& v, const Vector2& n) noexcept;
Vector3 ProjectAlongPlane(const Vector3& v, const Vector3& n) noexcept;
Vector4 ProjectAlongPlane(const Vector4& v, const Vector4& n) noexcept;

unsigned int CalculateManhattanDistance(const IntVector2& start, const IntVector2& end) noexcept;
unsigned int CalculateManhattanDistance(const IntVector3& start, const IntVector3& end) noexcept;
unsigned int CalculateManhattanDistance(const IntVector4& start, const IntVector4& end) noexcept;

unsigned int CalculateChebyshevDistance(const IntVector2& start, const IntVector2& end) noexcept;
unsigned int CalculateChebyshevDistance(const IntVector3& start, const IntVector3& end) noexcept;
unsigned int CalculateChebyshevDistance(const IntVector4& start, const IntVector4& end) noexcept;

unsigned int CalculateChessboardDistance(const IntVector2& start, const IntVector2& end) noexcept;
unsigned int CalculateChessboardDistance(const IntVector3& start, const IntVector3& end) noexcept;
unsigned int CalculateChessboardDistance(const IntVector4& start, const IntVector4& end) noexcept;

Vector2 GetRandomPointOn(const AABB2& aabb) noexcept;
Vector2 GetRandomPointOn(const Disc2& disc) noexcept;
Vector2 GetRandomPointOn(const LineSegment2& line) noexcept;

Vector3 GetRandomPointOn(const AABB3& aabb) noexcept;
Vector3 GetRandomPointOn(const Sphere3& sphere) noexcept;
Vector3 GetRandomPointOn(const LineSegment3& line) noexcept;

Vector2 GetRandomPointInside(const AABB2& aabb) noexcept;
Vector2 GetRandomPointInside(const Disc2& disc) noexcept;

Vector3 GetRandomPointInside(const AABB3& aabb) noexcept;
Vector3 GetRandomPointInside(const Sphere3& sphere) noexcept;

bool Contains(const AABB2& aabb, const Vector2& point) noexcept;
bool Contains(const AABB2& a, const AABB2& b) noexcept;
bool Contains(const AABB2& a, const OBB2& b) noexcept;
bool Contains(const OBB2& a, const AABB2& b) noexcept;
bool Contains(const OBB2& a, const OBB2& b) noexcept;

bool IsPointInside(const AABB2& aabb, const Vector2& point) noexcept;
bool IsPointInside(const AABB3& aabb, const Vector3& point) noexcept;
bool IsPointInside(const OBB2& obb, const Vector2& point) noexcept;
bool IsPointInside(const Disc2& disc, const Vector2& point) noexcept;
bool IsPointInside(const Capsule2& capsule, const Vector2& point) noexcept;
bool IsPointInside(const Polygon2& poly2, const Vector2& point) noexcept;
bool IsPointInside(const Sphere3& sphere, const Vector3& point) noexcept;
bool IsPointInside(const Capsule3& capsule, const Vector3& point) noexcept;

bool IsPointOn(const Disc2& disc, const Vector2& point) noexcept;
bool IsPointOn(const LineSegment2& line, const Vector2& point) noexcept;
bool IsPointOn(const Capsule2& capsule, const Vector2& point) noexcept;
bool IsPointOn(const LineSegment3& line, const Vector3& point) noexcept;
bool IsPointOn(const Sphere3& sphere, const Vector3& point) noexcept;
bool IsPointOn(const Capsule3& capsule, const Vector3& point) noexcept;
bool IsPointOn(const Polygon2& poly2, const Vector2& point) noexcept;

Vector2 CalcClosestPoint(const Vector2& p, const AABB2& aabb) noexcept;
Vector2 CalcClosestPoint(const Vector2& p, const OBB2& obb) noexcept;
Vector3 CalcClosestPoint(const Vector3& p, const AABB3& aabb) noexcept;
Vector2 CalcClosestPoint(const Vector2& p, const Polygon2& poly2) noexcept;
Vector2 CalcClosestPoint(const Vector2& p, const Disc2& disc) noexcept;
Vector2 CalcClosestPoint(const Vector2& p, const LineSegment2& line) noexcept;
Vector2 CalcClosestPoint(const Vector2& p, const Capsule2& capsule) noexcept;

Vector3 CalcClosestPoint(const Vector3& p, const LineSegment3& line) noexcept;
Vector3 CalcClosestPoint(const Vector3& p, const Sphere3& sphere) noexcept;
Vector3 CalcClosestPoint(const Vector3& p, const Capsule3& capsule) noexcept;

Vector2 CalcNormalizedPointFromPoint(const Vector2& pos, const AABB2& bounds) noexcept;
Vector2 CalcPointFromNormalizedPoint(const Vector2& uv, const AABB2& bounds) noexcept;
Vector2 CalcNormalizedHalfExtentsFromPoint(const Vector2& pos, const AABB2& bounds) noexcept;
Vector2 CalcPointFromNormalizedHalfExtents(const Vector2& uv, const AABB2& bounds) noexcept;

bool DoDiscsOverlap(const Disc2& a, const Disc2& b) noexcept;
bool DoDiscsOverlap(const Vector2& centerA, float radiusA, const Vector2& centerB, float radiusB) noexcept;
bool DoDiscsOverlap(const Disc2& a, const Capsule2& b) noexcept;
bool DoDiscsOverlap(const Disc2& a, const AABB2& b) noexcept;

bool DoSpheresOverlap(const Sphere3& a, const Sphere3& b) noexcept;
bool DoSpheresOverlap(const Vector3& centerA, float radiusA, const Vector3& centerB, float radiusB) noexcept;
bool DoSpheresOverlap(const Sphere3& a, const Capsule3& b) noexcept;
bool DoSpheresOverlap(const Sphere3& a, const AABB3& b) noexcept;

bool DoAABBsOverlap(const AABB2& a, const AABB2& b) noexcept;
bool DoAABBsOverlap(const AABB3& a, const AABB3& b) noexcept;
bool DoAABBsOverlap(const AABB2& a, const Disc2& b) noexcept;
bool DoAABBsOverlap(const AABB3& a, const Sphere3& b) noexcept;
bool DoOBBsOverlap(const OBB2& a, const OBB2& b) noexcept;
bool DoPolygonsOverlap(const Polygon2& a, const Polygon2& b) noexcept;

std::optional<Vector2> DoLineSegmentOverlap(const LineSegment2& a, const LineSegment2& b) noexcept;
bool DoLineSegmentOverlap(const Disc2& a, const LineSegment2& b) noexcept;
bool DoLineSegmentOverlap(const Sphere3& a, const LineSegment3& b) noexcept;

bool DoCapsuleOverlap(const Disc2& a, const Capsule2& b) noexcept;
bool DoCapsuleOverlap(const Sphere3& a, const Capsule3& b) noexcept;

bool DoPlaneOverlap(const Disc2& a, const Plane2& b) noexcept;
bool DoPlaneOverlap(const Sphere3& a, const Plane3& b) noexcept;
bool DoPlaneOverlap(const Capsule2& a, const Plane2& b) noexcept;
bool DoPlaneOverlap(const Capsule3& a, const Plane3& b) noexcept;

bool IsPointInFrontOfPlane(const Vector3& point, const Plane3& plane) noexcept;
bool IsPointBehindOfPlane(const Vector3& point, const Plane3& plane) noexcept;
bool IsPointOnPlane(const Vector3& point, const Plane3& plane) noexcept;

bool IsPointInFrontOfPlane(const Vector2& point, const Plane2& plane) noexcept;
bool IsPointBehindOfPlane(const Vector2& point, const Plane2& plane) noexcept;
bool IsPointOnPlane(const Vector2& point, const Plane2& plane) noexcept;

//Column major
float CalculateMatrix3Determinant(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) noexcept;

//Column major
float CalculateMatrix2Determinant(float m00, float m01, float m10, float m11) noexcept;

Quaternion SLERP(const Quaternion& a, const Quaternion& b, float t) noexcept;

template<typename T>
T Interpolate(const T& a, const T& b, float t) {
    return ((1.0f - t) * a) + (t * b);
}

template<>
Vector2 Interpolate(const Vector2& a, const Vector2& b, float t);

template<>
Vector3 Interpolate(const Vector3& a, const Vector3& b, float t);

template<>
Vector4 Interpolate(const Vector4& a, const Vector4& b, float t);

template<>
IntVector2 Interpolate(const IntVector2& a, const IntVector2& b, float t);

template<>
IntVector3 Interpolate(const IntVector3& a, const IntVector3& b, float t);

template<>
IntVector4 Interpolate(const IntVector4& a, const IntVector4& b, float t);

template<>
AABB2 Interpolate(const AABB2& a, const AABB2& b, float t);

template<>
AABB3 Interpolate(const AABB3& a, const AABB3& b, float t);

template<>
OBB2 Interpolate(const OBB2& a, const OBB2& b, float t);

template<>
Polygon2 Interpolate(const Polygon2& a, const Polygon2& b, float t);

template<>
Disc2 Interpolate(const Disc2& a, const Disc2& b, float t);

template<>
LineSegment2 Interpolate(const LineSegment2& a, const LineSegment2& b, float t);

template<>
Capsule2 Interpolate(const Capsule2& a, const Capsule2& b, float t);

template<>
LineSegment3 Interpolate(const LineSegment3& a, const LineSegment3& b, float t);

template<>
Sphere3 Interpolate(const Sphere3& a, const Sphere3& b, float t);

template<>
Capsule3 Interpolate(const Capsule3& a, const Capsule3& b, float t);

template<>
Plane2 Interpolate(const Plane2& a, const Plane2& b, float t);

template<>
Plane3 Interpolate(const Plane3& a, const Plane3& b, float t);

template<>
Quaternion Interpolate(const Quaternion& a, const Quaternion& b, float t);

template<>
Rgba Interpolate(const Rgba& a, const Rgba& b, float t);

template<typename T>
T RangeMap(const T& valueToMap, const T& minInputRange, const T& maxInputRange, const T& minOutputRange, const T& maxOutputRange) {
    return (valueToMap - minInputRange) * (maxOutputRange - minOutputRange) / (maxInputRange - minInputRange) + minOutputRange;
}

Vector2 RangeMap(const Vector2& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange);
Vector3 RangeMap(const Vector3& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange);
Vector4 RangeMap(const Vector4& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange);

IntVector2 RangeMap(const IntVector2& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange);
IntVector3 RangeMap(const IntVector3& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange);
IntVector4 RangeMap(const IntVector4& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange);

template<typename T>
T Wrap(const T& valueToWrap, const T& minValue, const T& maxValue) {
    T result = valueToWrap;
    while(result < minValue) {
        result += maxValue;
    }
    while(maxValue < result) {
        result -= maxValue;
    }
    return result;
}

template<>
Vector4 Wrap(const Vector4& valuesToWrap, const Vector4& minValues, const Vector4& maxValues);

template<>
Vector3 Wrap(const Vector3& valuesToWrap, const Vector3& minValues, const Vector3& maxValues);

template<>
Vector2 Wrap(const Vector2& valuesToWrap, const Vector2& minValues, const Vector2& maxValues);

template<>
IntVector4 Wrap(const IntVector4& valuesToWrap, const IntVector4& minValues, const IntVector4& maxValues);

template<>
IntVector3 Wrap(const IntVector3& valuesToWrap, const IntVector3& minValues, const IntVector3& maxValues);

template<>
IntVector2 Wrap(const IntVector2& valuesToWrap, const IntVector2& minValues, const IntVector2& maxValues);

namespace EasingFunctions {

namespace detail {

template<typename T, std::size_t... Is>
T SmoothStart_helper(const T& t, std::index_sequence<Is...>) {
    return (((void)Is, t) * ...);
}

template<typename T, std::size_t... Is>
T SmoothStop_helper(const T& t, std::index_sequence<Is...>) {
    return (((void)Is, (1.0f - t)) * ...);
}

} // namespace detail

template<std::size_t N, typename T>
T SmoothStart(const T& t) {
    static_assert(std::is_floating_point_v<T>, "SmoothStart requires T to be non-integral.");
    static_assert(N > 0, "SmoothStart requires value of N to be non-negative and non-zero.");
    return detail::SmoothStart_helper(t, std::make_index_sequence<N>{});
}

template<std::size_t N, typename T>
T SmoothStop(const T& t) {
    static_assert(std::is_floating_point_v<T>, "SmoothStop requires T to be non-integral.");
    static_assert(N > 0, "SmoothStop requires value of N to be non-negative and non-zero.");
    return detail::SmoothStop_helper(t, std::make_index_sequence<N>{});
}

template<std::size_t N, typename T>
T SmoothStep(const T& t) {
    static_assert(std::is_floating_point_v<T>, "SmoothStop requires T to be non-integral.");
    static_assert(N > 0, "SmoothStop requires value of N to be non-negative and non-zero.");
    return Interpolate(SmoothStart<N>(t), SmoothStop<N>(t), 0.5f);
}

} // namespace EasingFunctions

} // namespace MathUtils