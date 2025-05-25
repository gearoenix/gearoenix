#pragma once
#include "../../math/gx-math-aabb.hpp"
#include "gx-rnd-imgui-math-vec.hpp"
#include "gx-rnd-imgui-type-table.hpp"

namespace gearoenix::render::imgui {
template <typename T>
[[nodiscard]] bool show(math::Aabb3<T>& v)
{
    bool result = false;
    table_scope(&v, [&] {
        auto upper = v.get_upper();
        ImGui::Text("Upper:");
        ImGui::TableNextColumn();
        result |= render::imgui::show(upper);
        ImGui::TableNextColumn();

        auto lower = v.get_lower();
        ImGui::Text("Lower:");
        ImGui::TableNextColumn();
        result |= render::imgui::show(lower);
        ImGui::TableNextColumn();

        if (result) {
            v.reset(upper, lower);
        }

        ImGui::Text("Diameter:");
        ImGui::TableNextColumn();
        ImGui::Text("X: %lf, Y: %lf, Z: %lf", v.get_diameter().x, v.get_diameter().y, v.get_diameter().z);
        ImGui::TableNextColumn();

        ImGui::Text("Center:");
        ImGui::TableNextColumn();
        ImGui::Text("X: %lf, Y: %lf, Z: %lf", v.get_center().x, v.get_center().y, v.get_center().z);
        ImGui::TableNextColumn();

        ImGui::Text("Volume:");
        ImGui::TableNextColumn();
        ImGui::Text("%lf", v.get_volume());
    });
    return result;
}
}