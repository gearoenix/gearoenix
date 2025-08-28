#pragma once
#include "gx-rnd-imgui-id.hpp"
#include <ImGui/imgui.h>

namespace gearoenix::render::imgui {
template <typename Function>
void table_scope(const char* const id, Function&& func)
{
    if (!ImGui::BeginTable(id, 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit)) {
        return;
    }
    ImGui::TableSetupColumn("##labels", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("##inputs", ImGuiTableColumnFlags_WidthStretch, 0.5f);
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    func();
    ImGui::EndTable();
}

template <typename Function>
void table_scope(const void* const ptr, Function&& func)
{
    const auto id = create_id_string(ptr);
    table_scope(id.data(), std::forward<Function>(func));
}
}