#include "gx-ed-vp-button.hpp"

#include "../ui/gx-ed-ui-tooltip.hpp"

#include <gearoenix/core/gx-cr-string.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-2d.hpp>

void gearoenix::editor::viewport::Button::apply_current_scale()
{
    const float ui_font_size = ImGui::GetFontSize();
    const float scale = ui_font_size / reference_font_size;

    // Glyph sizes — the button's text uses exactly the UI font size so it reads as
    // part of the editor chrome rather than an oversized toolbar. Icons sit ~15 %
    // bigger so they dominate the button's glyph composition.
    text_font_size = ui_font_size;
    icon_size = ui_font_size * 1.15f;
    height = icon_size * 1.4f;
    icon_margin = (height - icon_size) * 0.5f;
    icon_end = icon_size + icon_margin;

    // Row anchor — `WorkPos.y` is the main menu bar's bottom edge, so this keeps the
    // whole toolbar row locked to the menu even when menu-bar height changes.
    const float menu_bar_bottom = ImGui::GetMainViewport()->WorkPos.y;
    top_margin = menu_bar_bottom + ui_font_size * 0.40f;

    // Non-glyph spacing still uses the legacy scale factor — these are layout
    // distances, not text metrics, so they don't need to match GetFontSize exactly.
    spacing = 10.0f * scale;
    rounding = 10.0f * scale;
}

void gearoenix::editor::viewport::Button::compute_values()
{
    end_point = { start_point.x + size.x, start_point.y + size.y };
    icon_start_point = { start_point.x + icon_margin, start_point.y + icon_margin };
    icon_end_point = { icon_start_point.x + icon_size, icon_start_point.y + icon_size };
    text_start_point = { icon_end_point.x + icon_margin, start_point.y + icon_margin };
}

void gearoenix::editor::viewport::Button::show()
{
    // Re-derive position + size every frame so the button follows the UI text-size
    // slider and the menu-bar height without any caching-related staleness bugs.
    apply_current_scale();
    start_point = { compute_start_x(), top_margin };
    size = compute_size();
    compute_values();

    ImGui::SetNextWindowPos(start_point, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    [[maybe_unused]] const auto is_window_open = ImGui::Begin(invisible_window_id.c_str(), nullptr,
        // clang-format off
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoNavInputs |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoTitleBar
        // clang-format on
    );

    GX_ASSERT_D(is_window_open);

    auto* const dl = ImGui::GetWindowDrawList();

    // Invisible button over the whole pill area handles the click/toggle interaction.
    if (ImGui::InvisibleButton(invisible_button_id.c_str(), size)) {
        toggled = !toggled;
        clicked_in_this_frame = true;
    } else {
        clicked_in_this_frame = false;
    }

    // Tooltip via the helper that delegates to `ImGui::SetItemTooltip` (1.89.7+) —
    // honours the style-driven hover delay and avoids the Begin/End-pairing trap.
    // Pick whichever string is set for the current toggle state; falling back keeps
    // a single-state (non-toggleable) button working when only `tooltip` is provided.
    ui::tooltip((toggled && toggled_tooltip) ? toggled_tooltip : tooltip);

    // Pill background + outline.
    dl->AddRectFilled(start_point, end_point, toggled ? toggled_background_colour : background_colour, rounding);
    dl->AddRect(start_point, end_point, border_colour, rounding, 0, 2.0f);

    // Icon (pick the right texture based on toggle state).
    void* icon_ptr = nullptr;
    if (icon && !toggled) {
        icon_ptr = icon->get_imgui_ptr();
    } else if (toggled_icon && toggled) {
        icon_ptr = toggled_icon->get_imgui_ptr();
    }
    if (nullptr != icon_ptr) {
        const auto tex_id = static_cast<ImTextureID>(reinterpret_cast<std::size_t>(icon_ptr));
        dl->AddImage(tex_id, icon_start_point, icon_end_point);
    }

    // Label (if any).
    if (toggled ? !toggled_text.empty() : !text.empty()) {
        dl->AddText(nullptr, text_font_size, text_start_point, text_colour, toggled ? toggled_text.c_str() : text.c_str());
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

gearoenix::editor::viewport::Button::Button()
    : invisible_window_id("##window-" + core::String::ptr_name(this))
    , invisible_button_id("##button-" + core::String::ptr_name(this))
{
}

gearoenix::editor::viewport::Button::~Button() = default;
