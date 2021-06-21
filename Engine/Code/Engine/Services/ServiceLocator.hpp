#pragma once

#include "Engine/Services/IAudioService.hpp"

#include <type_traits>

class IService;
class IAudioService;

class ServiceLocator {
public:
    template<typename ServiceInterface>
    static ServiceInterface& get() {
        static_assert(std::is_base_of_v<IService, ServiceInterface>, "Requested type is not a Service.");
        if constexpr (std::is_base_of_v<IAudioService, ServiceInterface>) {
            return *m_audioService;
        //} else if constexpr (std::is_base_of_v<>) {
        } else {
            return &m_nullService;
        }

    };

    template<typename ServiceInterface>
    static void provide(ServiceInterface* service) {
        static_assert(std::is_base_of_v<IService, ServiceInterface>, "Provided type is not a Service.");
        if constexpr(std::is_base_of_v<IAudioService, ServiceInterface>) {
            if(service == nullptr) {
                service = &m_nullAudioService;
            }
            m_audioService = service;
        }
    }
protected:
private:
    static inline NullAudioService m_nullService{};
    static inline IAudioService* m_audioService{nullptr};
};
