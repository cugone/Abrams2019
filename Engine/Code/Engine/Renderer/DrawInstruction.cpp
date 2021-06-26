#include "Engine/Renderer/DrawInstruction.hpp"

#include "Engine/Renderer/Material.hpp"

[[nodiscard]] bool DrawInstruction::operator==(const DrawInstruction& rhs) {
    return material == rhs.material && type == rhs.type && indexCount == rhs.indexCount && indexStart == rhs.indexStart && baseVertexLocation == rhs.baseVertexLocation;
}

[[nodiscard]] bool DrawInstruction::operator!=(const DrawInstruction& rhs) {
    return !(*this == rhs);
}