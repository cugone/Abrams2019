#pragma once

#include "Engine/Core/Vertex3D.hpp"
#include "Engine/RHI/RHITypes.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <vector>

class Mesh {
public:
    Mesh() = default;
    Mesh(const Mesh& other) = default;
    Mesh(Mesh&& other) = default;
    Mesh& operator=(const Mesh& other) = default;
    Mesh& operator=(Mesh&& other) = default;
    ~Mesh() = default;

    class Builder {
    public:
        enum class Primitive {
            Point, Line, Triangle, Quad
        };

        Builder() = default;
        Builder(const Builder& other) = default;
        Builder(Builder&& other) = default;
        Builder(const std::vector<Vertex3D>& verts, const std::vector<unsigned int>& indcs) noexcept;
        Builder& operator=(const Builder& other) = default;
        Builder& operator=(Builder&& other) = default;
        ~Builder() = default;

        std::vector<Vertex3D> verticies{};
        std::vector<unsigned int> indicies{};
        std::vector<Renderer::DrawInstruction> draw_instructions{};

        void Begin(const PrimitiveType& type) noexcept;
        void End(Material* mat = nullptr) noexcept;
        void Clear() noexcept;

        void SetTangent(const Vector3& tangent) noexcept;
        void SetBitangent(const Vector3& bitangent) noexcept;
        void SetNormal(const Vector3& normal) noexcept;
        void SetColor(const Rgba& color) noexcept;
        void SetColor(const Vector4& color) noexcept;
        void SetUV(const Vector2& uv) noexcept;

        std::size_t AddVertex(const Vector3& position) noexcept;
        std::size_t AddIndicies(const Primitive& type) noexcept;
    private:
        Vertex3D _vertex_prototype{};
        Renderer::DrawInstruction _current_draw_instruction{};
    };

protected:
private:
};
