#pragma once

#include "Engine/Renderer/Buffer.hpp"

#include "Engine/Core/Vertex3D.hpp"

#include <vector>

class RHIDevice;
class RHIDeviceContext;

class StructuredBuffer : public Buffer<void*> {
public:
    StructuredBuffer(const RHIDevice* owner, const buffer_t& buffer, std::size_t element_size, std::size_t element_count, const BufferUsage& usage, const BufferBindUsage& bindUsage) noexcept;
    virtual ~StructuredBuffer() noexcept;

    void Update(RHIDeviceContext* context, const buffer_t& buffer) noexcept;

    ID3D11ShaderResourceView* dx_srv = nullptr;
protected:
private:
    std::size_t _element_count = 0;
    std::size_t _element_size = 0;
    std::size_t _buffer_size = 0;
};
