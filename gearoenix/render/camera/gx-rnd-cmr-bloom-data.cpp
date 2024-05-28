#include "gx-rnd-cmr-bloom-data.hpp"
#include <imgui.h>

gearoenix::render::camera::BloomData::BloomData()
    : scatter_clamp_max_threshold_threshold_knee(0.5f, 7.0f, 0.9f, 0.5f)
{
}

void gearoenix::render::camera::BloomData::show_debug_data()
{
    if (ImGui::TreeNode("Bloom")) {
        ImGui::InputFloat("Scatter", &scatter_clamp_max_threshold_threshold_knee.x, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Clamp Max", &scatter_clamp_max_threshold_threshold_knee.y, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Threshold", &scatter_clamp_max_threshold_threshold_knee.z, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Threshold Knee", &scatter_clamp_max_threshold_threshold_knee.w, 0.01f, 1.0f, "%.3f");
        ImGui::TreePop();
    }
}
