#pragma once
#include "../../math/gx-math-vector-3d.hpp"
#include "gx-rnd-imgui-id.hpp"
#include <imgui.h>

namespace gearoenix::render::imgui {
template <typename T>
[[nodiscard]] bool show(math::Vec3<T>& v, const float item_width = 100.0f, const char* const item_format = "%.3f")
{
    const auto ptr_id = create_id_string(&v);
    constexpr auto eid_sz = std::size(ptr_id) + 2;
    constexpr auto eid_eli = eid_sz - 2;
    constexpr auto eid_di = eid_eli - 1;
    std::array<char, eid_sz> eid;
    eid[eid_di] = '.';
    eid[eid_sz - 1] = '\0';
    for (int i = 0; i < ptr_id.size() - 1; ++i) {
        eid[i] = ptr_id[i];
    }
    bool result = false;

    const auto fun = [&](T& e) constexpr {
        if constexpr (std::is_same_v<double, T>) {
            result |= ImGui::InputDouble(eid.data(), &e, 0.01, 1.0, item_format);
        } else if constexpr (std::is_same_v<float, T>) {
            result |= ImGui::InputFloat(eid.data(), &e, 0.01, 1.0, item_format);
        } else {
            static_assert(false, "Unimplemented type.");
        }
    };

    ImGui::AlignTextToFramePadding();
    ImGui::Text("X:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(item_width);
    eid[eid_eli] = 'x';
    fun(v.x);

    ImGui::SameLine();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Y:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(item_width);
    eid[eid_eli] = 'y';
    fun(v.y);

    ImGui::SameLine();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Z:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(item_width);
    eid[eid_eli] = 'z';
    fun(v.z);

    return result;
}
}