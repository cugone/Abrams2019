#pragma once

#include <type_traits>

namespace Utils {

    template<typename Callable>
    void DoOnce(Callable&& f, bool reset = false, bool start_closed = false) {
        static bool once = start_closed;
        if(!once) {
            once = true;
            std::invoke(f);
        }
        if(reset) {
            once = false;
        }
    }

    template<typename CallableT, typename CallableF>
    bool FlipFlop(CallableT&& f_true, CallableF&& f_false) noexcept {
        static bool value = true;
        bool is_true = value;
        value ? std::invoke(f_true) : std::invoke(f_false);
        value = !value;
        return is_true;
    }

    bool FlipFlop() noexcept {
        return FlipFlop([]() {}, []() {});
    }

}

