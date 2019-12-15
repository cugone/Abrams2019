#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"

#include <vector>

class Polygon2 {
public:
    Polygon2(int sides = 3, const Vector2& position = Vector2::ZERO, const Vector2& half_extents = Vector2(0.5f, 0.5f), float orientationDegrees = 0.0f)
        : _sides(sides)
        , _orientationDegrees(orientationDegrees)
        , _half_extents(half_extents)
        , _position(position) {
        CalcVerts();
        CalcNormals();
    }

    virtual ~Polygon2() = default;

    int GetSides() const {
        return _sides;
    }
    void SetSides(int sides) {
        if(_sides == sides) {
            return;
        }
        _sides = sides;
        CalcVerts();
        CalcNormals();
    }
    const Vector2& GetPosition() const {
        return _position;
    }
    void SetPosition(const Vector2& position) {
        _position = position;
        CalcVerts();
    }
    void Translate(const Vector2& translation) {
        _position += translation;
        CalcVerts();
    }
    void RotateDegrees(float displacementDegrees) {
        _orientationDegrees += displacementDegrees;
        _orientationDegrees = MathUtils::Wrap(_orientationDegrees, 0.0f, 360.0f);
        CalcVerts();
        CalcNormals();
    }
    void Rotate(float displacementRadians) {
        RotateDegrees(MathUtils::ConvertRadiansToDegrees(displacementRadians));
    }
    float GetOrientationDegrees() const {
        return _orientationDegrees;
    }
    void SetOrientationDegrees(float degrees) {
        _orientationDegrees = degrees;
        _orientationDegrees = MathUtils::Wrap(_orientationDegrees, 0.0f, 360.0f);
        CalcVerts();
        CalcNormals();
    }
    const std::vector<Vector2>& GetVerts() const {
        return _verts;
    }
    const std::vector<Vector2>& GetNormals() const {
        return _normals;
    }
    const Vector2& GetHalfExtents() const {
        return _half_extents;
    }
    void SetHalfExtents(const Vector2& newHalfExtents) {
        _half_extents = newHalfExtents;
    }
    void AddPaddingToSides(const Vector2& padding) {
        AddPaddingToSides(padding.x, padding.y);
    }
    void AddPaddingToSides(float paddingX, float paddingY) {
        _half_extents.x += paddingX;
        _half_extents.y += paddingY;
    }

protected:
    void CalcNormals() {
        auto s = _verts.size();
        _normals.clear();
        if(_normals.capacity() < s) {
            _normals.reserve(s);
        }
        for(std::size_t i = 0; i < s; ++i) {
            auto j = (i + 1) % s;
            auto n = (_verts[j] - _verts[i]).GetNormalize().GetLeftHandNormal();
            _normals.push_back(n);
        }
        const auto S = Matrix4::CreateScaleMatrix(_half_extents);
        const auto R = Matrix4::Create2DRotationDegreesMatrix(_orientationDegrees);
        const auto T = Matrix4::CreateTranslationMatrix(_position);
        const auto M = Matrix4::MakeSRT(S, R, T);
        for(auto& n : _normals) {
            n = M.TransformDirection(n);
        }
    }

    void CalcVerts() {
        auto num_sides_as_float = static_cast<float>(_sides);
        _verts.clear();
        if(_verts.capacity() < _sides) {
            _verts.reserve(_sides);
        }
        auto anglePerVertex = 360.0f / num_sides_as_float;
        for(float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
            float radians = MathUtils::ConvertDegreesToRadians(degrees);
            float pX = 0.5f * std::cos(radians);
            float pY = 0.5f * std::sin(radians);
            _verts.emplace_back(Vector2(pX, pY));
        }
        const auto S = Matrix4::CreateScaleMatrix(_half_extents);
        const auto R = Matrix4::Create2DRotationDegreesMatrix(_orientationDegrees);
        const auto T = Matrix4::CreateTranslationMatrix(_position);
        const auto M = Matrix4::MakeSRT(S, R, T);
        for(auto& v : _verts) {
            v = M.TransformPosition(v);
        }
    }

private:
    int _sides = 3;
    float _orientationDegrees = 0.0f;
    Vector2 _half_extents = Vector2(0.5f, 0.5f);
    Vector2 _position = Vector2::ZERO;
    std::vector<Vector2> _verts;
    std::vector<Vector2> _normals;
};
