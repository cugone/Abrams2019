#pragma once

#include "Engine/RHI/RHITypes.hpp"

#include <memory>

class RHIOutput;
class IntVector2;
class RHIDevice;
struct IDXGIDebug;

class RHIInstance {
public:
    static RHIInstance* const CreateInstance();
    static void DestroyInstance();

    std::unique_ptr<RHIDevice> CreateDevice() const noexcept;

protected:
    RHIInstance() = default;
    ~RHIInstance();

private:
    static RHIInstance* _instance;
    static IDXGIDebug* _debuggerInstance;
};