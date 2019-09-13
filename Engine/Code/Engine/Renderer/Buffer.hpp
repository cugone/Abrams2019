#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"

template<typename T>
class Buffer {
public:
    using buffer_t = T;
    virtual ~Buffer() noexcept = 0;
    ID3D11Buffer* GetDxBuffer() const noexcept;
    bool IsValid() const noexcept;

protected:
    ID3D11Buffer* _dx_buffer = nullptr;
private:
};

template<typename T>
Buffer<T>::~Buffer() noexcept {
    if(IsValid()) {
        _dx_buffer->Release();
        _dx_buffer = nullptr;
    }
}

template<typename T>
ID3D11Buffer* Buffer<T>::GetDxBuffer() const noexcept {
    return _dx_buffer;
}

template<typename T>
bool Buffer<T>::IsValid() const noexcept {
    return _dx_buffer != nullptr;
}
