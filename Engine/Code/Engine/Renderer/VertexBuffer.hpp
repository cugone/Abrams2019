#pragma once

#include "Engine/Renderer/ArrayBuffer.hpp"

#include "Engine/Core/Vertex3D.hpp"

#include <vector>

class RHIDevice;
class RHIDeviceContext;

class VertexBuffer : public ArrayBuffer<Vertex3D> {
public:
    VertexBuffer(const RHIDevice* owner, const buffer_t& buffer, const BufferUsage& usage, const BufferBindUsage& bindUsage);
    virtual ~VertexBuffer();

    void Update(RHIDeviceContext* context, const buffer_t& buffer);

protected:
private:
};
