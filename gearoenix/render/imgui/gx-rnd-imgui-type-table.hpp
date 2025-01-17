#pragma once
#include "gx-rnd-imgui-id.hpp"
#include <imgui/imgui.h>

namespace gearoenix::render::imgui {
template <typename Function>
void table_scope(const void* const ptr, Function&& func)
{
    const auto id = create_id_string(ptr);
    if (ImGui::BeginTable(id.data(), 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody)) {
        ImGui::TableSetupColumn("##labels", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("##inputs", ImGuiTableColumnFlags_WidthStretch, 0.999f);
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        func();
        ImGui::EndTable();
    }
}
}