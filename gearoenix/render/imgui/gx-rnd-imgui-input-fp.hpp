#pragma once
#include "../../core/gx-cr-build-configuration.hpp"

#include <ImGui/imgui.h>

#include <type_traits>

namespace gearoenix::render::imgui {
template <typename T>
[[nodiscard]] bool input(const char* const id, T& v, const T step = static_cast<T>(0), const T step_fast = static_cast<T>(0), const char* const format = "%.6f")
{
    static_assert(std::is_same_v<T, double> || std::is_same_v<T, float>);

    if constexpr (std::is_same_v<T, double>) {
        return ImGui::InputDouble(id, &v, step, step_fast, format);
    } else if constexpr (std::is_same_v<T, float>) {
        return ImGui::InputFloat(id, &v, step, step_fast, format);
    } else {
        return false;
    }
}
}