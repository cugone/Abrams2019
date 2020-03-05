#pragma once

#include "Engine/Core/Vertex3D.hpp"
#include "Engine/RHI/RHITypes.hpp"

#include <vector>

struct draw_instruction {
    PrimitiveType type{};
    std::size_t start_index{0};
    std::size_t count{0};
    bool uses_index_buffer{true};
};

bool operator==(const draw_instruction& a, const draw_instruction& b) noexcept;
bool operator!=(const draw_instruction& a, const draw_instruction& b) noexcept;

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
        Builder() = default;
        Builder(const Builder& other) = default;
        Builder(Builder&& other) = default;
        Builder(const std::vector<Vertex3D>& verts, const std::vector<unsigned int>& indcs) noexcept;
        Builder& operator=(const Builder& other) = default;
        Builder& operator=(Builder&& other) = default;
        ~Builder() = default;

        std::vector<Vertex3D> verticies{};
        std::vector<unsigned int> indicies{};
        std::vector<draw_instruction> draw_instructions{};

        void Begin(const PrimitiveType& type, bool hasIndexBuffer = true) noexcept;
        void End() noexcept;
        void Clear() noexcept;

        void SetTangent(const Vector3& tangent) noexcept;
        void SetBitangent(const Vector3& bitangent) noexcept;
        void SetNormal(const Vector3& normal) noexcept;
        void SetColor(const Rgba& color) noexcept;
        void SetColor(const Vector4& color) noexcept;
        void SetUV(const Vector2& uv) noexcept;

        std::size_t AddVertex(const Vector3& position) noexcept;

    private:
        Vertex3D _vertex_prototype{};
        draw_instruction _current_draw_instruction{};
    };

protected:
private:
};
