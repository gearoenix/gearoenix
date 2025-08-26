#include "gx-ed-vp-button.hpp"

#include <gearoenix/core/gx-cr-string.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-2d.hpp>

void gearoenix::editor::viewport::Button::show()
{
    ImGui::SetNextWindowPos(start_point, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    [[maybe_unused]] const auto is_window_open = ImGui::Begin(invisible_window_id.c_str(), nullptr,
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

    GX_ASSERT_D(is_window_open);

    auto* const dl = ImGui::GetWindowDrawList();

    if (ImGui::InvisibleButton(invisible_button_id.c_str(), size)) {
        toggled = !toggled;
        clicked_in_this_frame = true;
    } else {
        clicked_in_this_frame = false;
    }

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        if (tooltip && toggled_tooltip) {
            ImGui::TextUnformatted(toggled ? toggled_tooltip : tooltip);
        }
        ImGui::EndTooltip();
    }

    dl->AddRectFilled(start_point, end_point, toggled ? toggled_background_colour : background_colour, rounding);
    dl->AddRect(start_point, end_point, border_colour, rounding, 0, 2.0f);

    void* icon_ptr = nullptr;

    if (icon && !toggled) {
        icon_ptr = icon->get_imgui_ptr();
    } else if (toggled_icon && toggled) {
        icon_ptr = toggled_icon->get_imgui_ptr();
    }

    if (nullptr != icon_ptr) {
        dl->AddImage(static_cast<ImTextureID>(reinterpret_cast<std::size_t>(icon_ptr)), icon_start_point, icon_end_point, { 0, 1 }, { 1, 0 });
    }

    if (toggled ? !toggled_text.empty() : !text.empty()) {
        dl->AddText(nullptr, text_font_size, text_start_point, text_colour, toggled ? toggled_text.c_str() : text.c_str());
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void gearoenix::editor::viewport::Button::compute_values()
{
    end_point = { start_point.x + size.x, start_point.y + size.y };
    icon_start_point = { start_point.x + icon_margin, start_point.y + icon_margin };
    icon_end_point = { icon_start_point.x + icon_size, icon_start_point.y + icon_size };
    text_start_point = { icon_end_point.x + icon_margin, start_point.y + icon_margin };
}

gearoenix::editor::viewport::Button::Button()
    : invisible_window_id("##window-" + core::String::ptr_name(this))
    , invisible_button_id("##button-" + core::String::ptr_name(this))
{
}

gearoenix::editor::viewport::Button::~Button() = default;