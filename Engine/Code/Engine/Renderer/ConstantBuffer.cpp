#include "Engine/Renderer/ConstantBuffer.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

#include <sstream>

ConstantBuffer::ConstantBuffer(const RHIDevice* owner, const buffer_t& buffer, const std::size_t& buffer_size, const BufferUsage& usage, const BufferBindUsage& bindUsage) noexcept
    : Buffer<void*>()
    , _buffer_size(buffer_size)
{
    if(_buffer_size % 16) {
        ERROR_AND_DIE("Constant Buffer size not a multiple of 16.")
    }
    if(D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT < _buffer_size) {
        std::ostringstream ss;
        ss << "Constant Buffer of size " << _buffer_size << " exceeds maximum of " << D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT << '\n';
        ERROR_AND_DIE(ss.str().c_str());
    }

    D3D11_BUFFER_DESC buffer_desc = {};
    buffer_desc.Usage = BufferUsageToD3DUsage(usage);
    buffer_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    buffer_desc.CPUAccessFlags = CPUAccessFlagFromUsage(usage);
    buffer_desc.StructureByteStride = 0;
    buffer_desc.ByteWidth = static_cast<unsigned int>(_buffer_size);
    //MiscFlags are unused.

    D3D11_SUBRESOURCE_DATA init_data = {};
    init_data.pSysMem = buffer;

    _dx_buffer = nullptr;
    HRESULT hr = owner->GetDxDevice()->CreateBuffer(&buffer_desc, &init_data, &_dx_buffer);
    bool succeeded = SUCCEEDED(hr);
    if(!succeeded) {
        ERROR_AND_DIE("ConstantBuffer failed to create.");
    }
}

ConstantBuffer::~ConstantBuffer() noexcept {
    if(IsValid()) {
        _dx_buffer->Release();
        _dx_buffer = nullptr;
    }
}

void ConstantBuffer::Update(RHIDeviceContext* context, const buffer_t& buffer) noexcept {
    D3D11_MAPPED_SUBRESOURCE resource = {};
    auto dx_context = context->GetDxContext();
    HRESULT hr = dx_context->Map(_dx_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        std::memcpy(resource.pData, buffer, _buffer_size);
        dx_context->Unmap(_dx_buffer, 0);
    }
}
