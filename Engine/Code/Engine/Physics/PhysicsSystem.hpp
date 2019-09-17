#pragma once

#include "Engine/Core/ThreadSafeQueue.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/Vertex3D.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <atomic>
#include <algorithm>
#include <condition_variable>
#include <numeric>
#include <thread>
#include <vector>

class ColliderPolygon {
public:
    ColliderPolygon() {
        CalcVerts();
        CalcNormals();
    }

    explicit ColliderPolygon(int sides = 4, const Vector2& position = Vector2::ZERO, const Vector2& half_extents = Vector2(0.5f, 0.5f), float orientationDegrees = 0.0f)
        : _sides(sides)
        , _orientationDegrees(orientationDegrees)
        , _half_extents(half_extents)
        , _position(position)
	{
        CalcVerts();
        CalcNormals();
	}

    virtual ~ColliderPolygon() = default;
	void DebugRender(Renderer* renderer) const {
		if (!renderer) {
			return;
		}
        const std::vector<Vertex3D> vbo = [this]() {
            std::vector<Vertex3D> buffer;
            buffer.reserve(_verts.size());
            for(const auto& v : _verts) {
                buffer.push_back(Vertex3D(Vector3(v, 0.0f)));
            }
            return buffer;
        }();
        const std::vector<unsigned int> ibo = [this, &vbo]() {
            std::vector<unsigned int> buffer(vbo.size() + 1);
            std::iota(std::begin(buffer), std::end(buffer), 0u);
            buffer.back() = 0;
            return buffer;
        }();
        renderer->DrawIndexed(PrimitiveType::LinesStrip, vbo, ibo);
	}
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
    void Rotate(float displacementDegrees) {
        _orientationDegrees += displacementDegrees;
        _orientationDegrees = MathUtils::Wrap(_orientationDegrees, 0.0f, 360.0f);
        CalcVerts();
        CalcNormals();
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
        Matrix4 S = Matrix4::CreateScaleMatrix(_half_extents);
        Matrix4 R = Matrix4::Create2DRotationDegreesMatrix(_orientationDegrees);
        Matrix4 T = Matrix4::CreateTranslationMatrix(_position);
        Matrix4 M = T * R * S;
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
        Matrix4 S = Matrix4::CreateScaleMatrix(_half_extents);
        Matrix4 R = Matrix4::Create2DRotationDegreesMatrix(_orientationDegrees);
        Matrix4 T = Matrix4::CreateTranslationMatrix(_position);
        Matrix4 M = T * R * S;
        for(auto& v : _verts) {
            v = M.TransformPosition(v);
        }
    }

	int _sides = 4;
    float _orientationDegrees = 0.0f;
    Vector2 _half_extents = Vector2(0.5f, 0.5f);
    Vector2 _position = Vector2::ZERO;
	std::vector<Vector2> _verts;
	std::vector<Vector2> _normals;
private:
};

class ColliderOBB : public ColliderPolygon {
public:
	ColliderOBB(const Vector2& position, const Vector2& half_extents) : ColliderPolygon(4, position, half_extents, 0.0f) {
        /* DO NOTHING */
	}
protected:
private:
};

struct PhysicsObject {
	Matrix4 transform{};
	Polygon2 collider;
	PhysicsObject* parent = nullptr;
	std::vector<PhysicsObject*> children{};
	bool is_colliding = false;
	bool enable_physics = false;
	bool is_awake = false;
};

struct PhysicsSystemDesc {
	Vector2 worldsize = Vector2{1000.0f, 1000.0f};
	float gravity = 0.0980665f;
	float world_to_meters = 100.0f;
	int position_solver_iterations = 1;
	int velocity_solver_iterations = 1;
};

class PhysicsSystem {
public:
    PhysicsSystem(const PhysicsSystemDesc& desc = PhysicsSystemDesc{});
	~PhysicsSystem();

	void Initialize() noexcept;
	void BeginFrame() noexcept;
	void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
	void Render() const noexcept;
	void EndFrame() noexcept;

protected:
private:
	void Update_Worker(TimeUtils::FPSeconds deltaSeconds) noexcept;

	PhysicsSystemDesc desc;
	std::thread _update_thread;
	std::condition_variable _signal;
	std::atomic_bool _is_running;
	ThreadSafeQueue<PhysicsObject*> _physicsObjects;
};
