#pragma once

#include "Engine/Renderer/ArrayBuffer.hpp"

#include "Engine/Core/Vertex3D.hpp"

#include <vector>

class RHIDevice;
class RHIDeviceContext;

class IndexBuffer : public ArrayBuffer<unsigned int> {
public:
    IndexBuffer(const RHIDevice* owner, const buffer_t& buffer, const BufferUsage& usage, const BufferBindUsage& bindUsage);
    virtual ~IndexBuffer();

    void Update(RHIDeviceContext* context, const buffer_t& buffer);

protected:
private:
};
