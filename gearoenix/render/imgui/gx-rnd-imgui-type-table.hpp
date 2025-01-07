#pragma once
#include <imgui/imgui.h>

namespace gearoenix::render::imgui {
template <typename Function>
void table_scope(const char* const table_id, Function&& func)
{
    if (ImGui::BeginTable(table_id, 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody)) {
        ImGui::TableSetupColumn("##labels", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("##inputs", ImGuiTableColumnFlags_WidthStretch, 0.999f);
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        func();
        ImGui::EndTable();
    }
}
}