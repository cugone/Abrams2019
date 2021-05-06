#pragma once

#include "Engine/Renderer/Buffer.hpp"

#include <vector>

namespace a2de {

    class RHIDevice;
    class RHIDeviceContext;

    template<typename T>
    class ArrayBuffer : public Buffer<std::vector<T>> {
    public:
        using arraybuffer_t = T;
        virtual ~ArrayBuffer() noexcept = 0 {};

    protected:
    private:
    };

} // namespace a2de
