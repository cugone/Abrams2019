#include "Engine/Renderer/Mesh.hpp"

#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"

#include <vector>
#include <type_traits>

Mesh::Builder::Builder(const std::vector<Vertex3D>& verts, const std::vector<unsigned int>& indcs) noexcept
: verticies{verts}
, indicies{indcs} {
    /* DO NOTHING */
}

void Mesh::Builder::Begin(const PrimitiveType& type) noexcept {
    _current_draw_instruction.type = type;
    _current_draw_instruction.vertexStart = verticies.size();
    _current_draw_instruction.indexStart = indicies.size();
}

void Mesh::Builder::End(Material* mat /* = nullptr */) noexcept {
    _current_draw_instruction.material = mat;
    _current_draw_instruction.count = indicies.size() - _current_draw_instruction.indexStart;
    if(!draw_instructions.empty()) {
        auto& last_inst = draw_instructions.back();
        if(!mat) {
            _current_draw_instruction.material = last_inst.material;
        }
        if(last_inst == _current_draw_instruction) {
            last_inst.count += _current_draw_instruction.count;
        }
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

std::size_t Mesh::Builder::AddVertex(const Vector2& position) noexcept {
    return AddVertex(Vector3{position, 0.0f});
}

std::size_t Mesh::Builder::AddIndicies(const Primitive& type) noexcept {
    switch(type) {
    case Primitive::Point:
        indicies.push_back(static_cast<unsigned int>(verticies.size()) - 1u);
        break;
    case Primitive::Line:
    {
        const auto v_s = verticies.size();
        indicies.push_back(static_cast<unsigned int>(v_s) - 2);
        indicies.push_back(static_cast<unsigned int>(v_s) - 1);
        break;
    }
    case Primitive::Triangle:
    {
        const auto v_s = verticies.size();
        indicies.push_back(static_cast<unsigned int>(v_s) - 3u);
        indicies.push_back(static_cast<unsigned int>(v_s) - 2u);
        indicies.push_back(static_cast<unsigned int>(v_s) - 1u);
        break;
    }
    case Primitive::TriangleStrip:
    {
        const auto v_s = verticies.size();
        indicies.push_back(static_cast<unsigned int>(v_s) - 4u);
        indicies.push_back(static_cast<unsigned int>(v_s) - 3u);
        indicies.push_back(static_cast<unsigned int>(v_s) - 2u);
        indicies.push_back(static_cast<unsigned int>(v_s) - 1u);
        break;
    }
    case Primitive::Quad:
    {
        const auto v_s = verticies.size();
        indicies.push_back(static_cast<unsigned int>(v_s) - 4u);
        indicies.push_back(static_cast<unsigned int>(v_s) - 3u);
        indicies.push_back(static_cast<unsigned int>(v_s) - 2u);
        indicies.push_back(static_cast<unsigned int>(v_s) - 4u);
        indicies.push_back(static_cast<unsigned int>(v_s) - 2u);
        indicies.push_back(static_cast<unsigned int>(v_s) - 1u);
        break;
    }
    default:
        break;
    }
    return indicies.size() - 1;
}

void Mesh::Render(Renderer& renderer, const Mesh::Builder& builder) noexcept {
    for(const auto& draw_inst : builder.draw_instructions) {
        renderer.SetMaterial(draw_inst.material);
        auto cbs = draw_inst.material->GetShader()->GetConstantBuffers();
        const auto cb_size = cbs.size();
        for(int i = 0; i < cb_size; ++i) {
            renderer.SetConstantBuffer(renderer.CONSTANT_BUFFER_START_INDEX + i, &(cbs.begin() + i)->get());
        }
        renderer.DrawIndexed(draw_inst.type, builder.verticies, builder.indicies, draw_inst.count, draw_inst.vertexStart, draw_inst.baseVertexLocation);
        for(int i = 0; i < cb_size; ++i) {
            renderer.SetConstantBuffer(renderer.CONSTANT_BUFFER_START_INDEX + i, nullptr);
        }
    }
}

void Mesh::Render(Renderer& renderer) const noexcept {
    Render(renderer, m_builder);
}
