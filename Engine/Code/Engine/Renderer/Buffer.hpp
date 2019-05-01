#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"

template<typename T>
class Buffer {
public:
    using buffer_t = T;
    virtual ~Buffer() = 0;
    ID3D11Buffer* GetDxBuffer() const;
    bool IsValid() const;

protected:
    ID3D11Buffer* _dx_buffer = nullptr;
private:
};

template<typename T>
Buffer<T>::~Buffer() {
    if(IsValid()) {
        _dx_buffer->Release();
        _dx_buffer = nullptr;
    }
}

template<typename T>
ID3D11Buffer* Buffer<T>::GetDxBuffer() const {
    return _dx_buffer;
}

template<typename T>
bool Buffer<T>::IsValid() const {
    return _dx_buffer != nullptr;
}
