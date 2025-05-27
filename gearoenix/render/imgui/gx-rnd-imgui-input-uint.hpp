#pragma once
#include "../../core/macro/gx-cr-mcr-stringifier.hpp"
#include <imgui.h>

#define GX_IMGUI_VAR_INPUT_UINT(x)                       \
    [&] {                                                \
        auto v = static_cast<int>(x);                    \
        if (ImGui::InputInt("##" GX_STRINGIFY(x), &v)) { \
            if (v < 0) {                                 \
                v = 0;                                   \
            }                                            \
            x = static_cast<decltype(x)>(v);             \
            return true;                                 \
        }                                                \
        return false;                                    \
    }()
