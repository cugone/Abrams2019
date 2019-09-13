#include "Engine/RHI/RHIInstance.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIOutput.hpp"

RHIInstance* RHIInstance::_instance = nullptr;
IDXGIDebug* RHIInstance::_debuggerInstance = nullptr;

RHIInstance* const RHIInstance::CreateInstance() noexcept {
    if(_instance) {
        return _instance;
    }
    _instance = new RHIInstance();

    _instance->_debuggerInstance = nullptr;

#if defined(RENDER_DEBUG)

    HMODULE debug_module = nullptr;

    // Debug Setup

    debug_module = ::LoadLibraryA("Dxgidebug.dll");
    if(debug_module) {
        using GetDebugModuleCB = HRESULT(WINAPI *)(REFIID, void**);
        GetDebugModuleCB cb = (GetDebugModuleCB) ::GetProcAddress(debug_module, "DXGIGetDebugInterface");
        HRESULT hr = cb(__uuidof(IDXGIDebug), (void**)&_instance->_debuggerInstance);
        bool succeeded = SUCCEEDED(hr);
        ASSERT_OR_DIE(succeeded, "DXGIDugger failed to initialize.");
        _instance->_debuggerInstance->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL));
    }
#endif
    return _instance;
}

void RHIInstance::DestroyInstance() noexcept {
    if(_instance) {
        delete _instance;
        _instance = nullptr;
    }
}

std::unique_ptr<RHIDevice> RHIInstance::CreateDevice() const noexcept {
    return std::move(std::make_unique<RHIDevice>());
}

RHIInstance::~RHIInstance() noexcept {
#ifdef RENDER_DEBUG
    if(_instance && _instance->_debuggerInstance) {
        _instance->_debuggerInstance->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL));
        _instance->_debuggerInstance->Release();
        _instance->_debuggerInstance = nullptr;
    }
#endif
}
