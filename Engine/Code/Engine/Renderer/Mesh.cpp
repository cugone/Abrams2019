#include "Engine/Renderer/Mesh.hpp"

bool operator==(const draw_instruction& a, const draw_instruction& b) noexcept {
    return a.type == b.type && a.uses_index_buffer == b.uses_index_buffer;
}

bool operator!=(const draw_instruction& a, const draw_instruction& b) noexcept {
    return !(a == b);
}

Mesh::Builder::Builder(const std::vector<Vertex3D>& verts, const std::vector<unsigned int>& indcs) noexcept
: verticies{verts}
, indicies{indcs} {
    /* DO NOTHING */
}

void Mesh::Builder::Begin(const PrimitiveType& type, bool hasIndexBuffer /*= true*/) noexcept {
    _current_draw_instruction.type = type;
    _current_draw_instruction.uses_index_buffer = hasIndexBuffer;
    _current_draw_instruction.start_index = verticies.size();
}

void Mesh::Builder::End() noexcept {
    _current_draw_instruction.count = verticies.size() - _current_draw_instruction.start_index;
    auto& last_inst = draw_instructions.back();
    if(!draw_instructions.empty() && last_inst == _current_draw_instruction) {
        last_inst.count += _current_draw_instruction.count;
    } else {
        draw_instructions.push_back(_current_draw_instruction);
    }
}

void Mesh::Builder::Clear() noexcept {
    verticies.clear();
    indicies.clear();
    draw_instructions.clear();
}

void Mesh::Builder::SetTangent(const Vector3& tangent) noexcept {
    _vertex_prototype.tangent = tangent;
}

void Mesh::Builder::SetBitangent(const Vector3& bitangent) noexcept {
    _vertex_prototype.bitangent = bitangent;
}

void Mesh::Builder::SetNormal(const Vector3& normal) noexcept {
    _vertex_prototype.normal = normal;
}

void Mesh::Builder::SetColor(const Rgba& color) noexcept {
    SetColor(color.GetRgbaAsFloats());
}

void Mesh::Builder::SetColor(const Vector4& color) noexcept {
    _vertex_prototype.color = color;
}

void Mesh::Builder::SetUV(const Vector2& uv) noexcept {
    _vertex_prototype.texcoords = uv;
}

std::size_t Mesh::Builder::AddVertex(const Vector3& position) noexcept {
    _vertex_prototype.position = position;
    verticies.push_back(_vertex_prototype);
    return verticies.size() - 1;
}
