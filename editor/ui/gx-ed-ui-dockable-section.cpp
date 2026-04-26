#include "gx-ed-ui-dockable-section.hpp"

#include "gx-ed-ui-icons.hpp"
#include "gx-ed-ui-tooltip.hpp"

#include <ImGui/imgui.h>

#include <algorithm>
#include <string>

void gearoenix::editor::ui::DockableSection::draw_header(const float avail_w)
{
    const char* const title_cstr = title.c_str();
    const auto& style = ImGui::GetStyle();

    // --- Row layout ---------------------------------------------------------------
    //   [title]   [section-specific inline content]   [dock-toggle icon button]
    // The dock-toggle button is right-aligned; the inline content fills everything
    // between the title and the button.
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(title_cstr);

    const char* const icon = attached ? ICON_POP_OUT : ICON_DOCK;
    const char* const tip = attached ? "Pop out into a floating window" : "Dock back into the side panel";
    const float title_w = ImGui::CalcTextSize(title_cstr).x;
    // The dock button is a square whose side equals the standard frame height, so it
    // aligns with the search / name widget on the same row *and* its hover highlight
    // renders as a square tile around the icon rather than a wide rectangle.
    const float btn_size = ImGui::GetFrameHeight();
    const float gap = style.ItemSpacing.x;

    // Leave 1.5 * ItemSpacing.x between the inline content's right edge and the dock
    // button so the two don't visually touch when e.g. the entity search input is
    // focused and draws its active frame.
    const float inline_w = std::max(0.0f, avail_w - title_w - btn_size - 2.5f * gap);
    if (inline_w > 0.0f) {
        ImGui::SameLine();
        draw_header_inline_content(inline_w);
    }

    // Pin the dock-toggle to the right edge regardless of what (if anything) the
    // section drew between the title and here — SameLine with an absolute x uses
    // the current window's content-region coordinates.
    ImGui::SameLine(std::max(0.0f, avail_w - btn_size));

    // Transparent background so the button reads as a pure icon at rest; hover /
    // active tints come through via the theme's Button colours on the full square.
    // ImGui centres the glyph inside the square automatically via `RenderTextClipped`.
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

    // Reused scratch `std::string` — zero-alloc in steady state.
    static std::string button_id;
    button_id.clear();
    button_id.append(icon);
    button_id.append("##dock-toggle-");
    button_id.append(title_cstr);
    if (ImGui::Button(button_id.c_str(), ImVec2(btn_size, btn_size))) {
        attached = !attached;
    }
    ImGui::PopStyleColor();
    tooltip(tip);
}

void gearoenix::editor::ui::DockableSection::draw_docked_body()
{
    draw_header(ImGui::GetContentRegionAvail().x);
    ImGui::Separator();
    update();
}

void gearoenix::editor::ui::DockableSection::draw_as_floating_window()
{
    if (!shown || attached) {
        return;
    }
    // `Appearing` applies the stored pos / size exactly when the window transitions
    // hidden → visible (first pop-out, and every re-pop-out after being re-docked),
    // but leaves ImGui alone while the user drags / resizes. Reading back via
    // `GetWindowPos` / `GetWindowSize` after `Begin` keeps the stored values in
    // sync, so the next pop-out resumes where the user left off.
    ImGui::SetNextWindowPos(floating_pos, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(floating_size, ImGuiCond_Appearing);

    // Reused scratch `std::string` for the ImGui id — zero-alloc in steady state.
    // Safe because the editor UI is single-threaded and ImGui consumes the id
    // synchronously inside `Begin`.
    static std::string window_id;
    window_id.clear();
    window_id.append(title);
    window_id.append("##float");

    bool open = shown;
    if (ImGui::Begin(window_id.c_str(), &open, ImGuiWindowFlags_NoSavedSettings)) {
        floating_pos = ImGui::GetWindowPos();
        floating_size = ImGui::GetWindowSize();
        draw_docked_body();
    }
    ImGui::End();
    // Reflect the user clicking the window's X button back into the shown flag.
    shown = open;
}
