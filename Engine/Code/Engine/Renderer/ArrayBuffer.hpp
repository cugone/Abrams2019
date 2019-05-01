#pragma once

#include "Engine/Renderer/Buffer.hpp"

#include <vector>

class RHIDevice;
class RHIDeviceContext;

template<typename T>
class ArrayBuffer : public Buffer<std::vector<T>> {
public:
    using arraybuffer_t = T;
    virtual ~ArrayBuffer() = 0 {};

protected:
private:
};
