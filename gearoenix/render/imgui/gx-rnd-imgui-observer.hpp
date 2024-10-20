#ifndef GEAROENIX_RENDER_IMGUI_OBSERVER_HPP
#define GEAROENIX_RENDER_IMGUI_OBSERVER_HPP
#include "../../core/macro/gx-cr-mcr-stringifier.hpp"
#include <imgui/imgui.h>

#define GX_IMGUI_OBSERVER(ob, show_function) \
    [&] {                                    \
        auto v = ob.get();                   \
        if (show_function(v)) {              \
            ob.set(v);                       \
            return true;                     \
        }                                    \
        return false;                        \
    }()

#endif