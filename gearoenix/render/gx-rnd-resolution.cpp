#include "gx-rnd-resolution.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "imgui/gx-rnd-imgui-input-uint.hpp"

namespace {
constexpr const char* names[] = {
    "Fixed",
    "Screen Based"
};
}

const gearoenix::render::Resolution::Fixed& gearoenix::render::Resolution::get_fixed() const
{
    GX_ASSERT(index == fixed_index);
    return data.fixed;
}

const gearoenix::render::Resolution::ScreenBased& gearoenix::render::Resolution::get_screen_based() const
{
    GX_ASSERT(index == screen_based_index);
    return data.screen_based;
}

bool gearoenix::render::Resolution::show_debug_gui()
{
    bool result = false;
    auto type_index = static_cast<int>(index);
    ImGui::SetNextItemWidth(200);
    if (ImGui::Combo("##resolution_type", &type_index, names, std::size(names))) {
        if (static_cast<int>(index) != type_index) {
            result = true;
            index = static_cast<index_t>(type_index);
            switch (index) {
            case fixed_index:
                data.fixed = {};
                break;
            case screen_based_index:
                data.screen_based = {};
                break;
            default:
                GX_UNEXPECTED;
            }
        }
    }
    ImGui::SameLine();
    switch (index) {
    case fixed_index: {
        ImGui::Text("Width:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        result |= GX_IMGUI_VAR_INPUT_UINT(data.fixed.width);
        ImGui::SameLine();
        ImGui::Text("Height:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        result |= GX_IMGUI_VAR_INPUT_UINT(data.fixed.height);
    } break;
    case screen_based_index: {
        ImGui::Text("Numerator:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        result |= GX_IMGUI_VAR_INPUT_UINT(data.screen_based.numerator);
        ImGui::SameLine();
        ImGui::Text("Denominator:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        result |= GX_IMGUI_VAR_INPUT_UINT(data.screen_based.denominator);
    } break;
    default:
        GX_UNEXPECTED;
    }
    return result;
}

void gearoenix::render::Resolution::write(platform::stream::Stream& stream) const
{
    stream.write_fail_debug(index);
    switch (index) {
    case fixed_index: {
        stream.write_fail_debug(data.fixed.width);
        stream.write_fail_debug(data.fixed.height);
        break;
    }
    case screen_based_index: {
        stream.write_fail_debug(data.screen_based.numerator);
        stream.write_fail_debug(data.screen_based.denominator);
        break;
    }
    default:
        GX_UNEXPECTED;
    }
}

void gearoenix::render::Resolution::read(platform::stream::Stream& stream)
{
    index = stream.read<index_t>();
    switch (index) {
    case fixed_index:
        stream.read(data.fixed.width);
        stream.read(data.fixed.height);
        break;
    case screen_based_index:
        stream.read(data.screen_based.numerator);
        stream.read(data.screen_based.denominator);
        break;
    default:
        GX_UNEXPECTED;
    }
}