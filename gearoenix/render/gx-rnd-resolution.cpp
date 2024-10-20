#include "gx-rnd-resolution.hpp"
#include "imgui/gx-rnd-imgui-input-uint.hpp"
#include <array>
#include <boost/mp11/algorithm.hpp>

namespace {
constexpr auto names = std::array {
    "FixedResolution",
    "ScreenBasedResolution"
};
}

bool gearoenix::render::imgui_show(Resolution& r)
{
    bool result = false;
    auto type_index = static_cast<int>(r.index());
    ImGui::SetNextItemWidth(200);
    if (ImGui::Combo("##resolution_type", &type_index, names.data(), static_cast<int>(names.size()))) {
        if (static_cast<int>(r.index()) != type_index) {
            result = true;
            switch (type_index) {
            case boost::mp11::mp_find<Resolution, FixedResolution>::value:
                r = FixedResolution {};
                break;
            case boost::mp11::mp_find<Resolution, ScreenBasedResolution>::value:
                r = ScreenBasedResolution {};
                break;
            }
        }
    }
    ImGui::SameLine();
    switch (r.index()) {
    case boost::mp11::mp_find<Resolution, FixedResolution>::value: {
        auto& [width, height] = std::get<FixedResolution>(r);
        ImGui::Text("Width:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        result |= GX_IMGUI_VAR_INPUT_UINT(width);
        ImGui::SameLine();
        ImGui::Text("Height:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        result |= GX_IMGUI_VAR_INPUT_UINT(height);
    } break;
    case boost::mp11::mp_find<Resolution, ScreenBasedResolution>::value: {
        auto& [num, den] = std::get<ScreenBasedResolution>(r);
        ImGui::Text("Numerator:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        result |= GX_IMGUI_VAR_INPUT_UINT(num);
        ImGui::SameLine();
        ImGui::Text("Denominator:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        result |= GX_IMGUI_VAR_INPUT_UINT(den);
    } break;
    }
    return result;
}
