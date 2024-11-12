#ifndef GEAROENIX_EDITOR_VIEWPORT_BUTTON_HPP
#define GEAROENIX_EDITOR_VIEWPORT_BUTTON_HPP
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <imgui/imgui.h>
#include <memory>
#include <string>

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
struct Button {
    const std::string invisible_window_id, invisible_button_id;

    GX_GET_CREF_PRT(ImVec2, start_point);
    GX_GET_CREF_PRT(ImVec2, size);
    GX_GET_CREF_PRT(ImVec2, end_point);
    GX_GET_CREF_PRT(ImVec2, icon_start_point);
    GX_GET_CREF_PRT(ImVec2, icon_end_point);
    GX_GET_CREF_PRT(ImVec2, text_start_point);
    GX_GET_CREF_PRT(std::string, text);
    GX_GET_CREF_PRT(std::string, toggled_text);
    GX_GET_CREF_PRT(std::shared_ptr<render::texture::Texture2D>, icon);
    GX_GET_CREF_PRT(std::shared_ptr<render::texture::Texture2D>, toggled_icon);
    GX_GET_CPTR_PRT(char, tooltip);
    GX_GET_CPTR_PRT(char, toggled_tooltip);
    GX_GET_VAL_PRT(bool, toggled, false);
    GX_GET_VAL_PRT(bool, is_toggleable, false);
    GX_GET_VAL_PRT(bool, clicked_in_this_frame, false);
    GX_GET_VAL_PRT(float, icon_size, 20.0f);
    GX_GET_VAL_PRT(float, height, icon_size * 1.4f);
    GX_GET_VAL_PRT(float, icon_margin, (height - icon_size) * 0.5f);
    GX_GET_VAL_PRT(float, icon_end, icon_size + icon_margin);
    GX_GET_VAL_PRT(float, top_margin, 30.0f);
    GX_GET_VAL_PRT(float, spacing, 10.0f);
    GX_GET_VAL_PRT(float, text_font_size, 20.0f);
    GX_GET_VAL_PRT(float, rounding, 10.0f);
    GX_GET_VAL_PRT(ImU32, text_colour, IM_COL32(200, 200, 200, 255));
    GX_GET_VAL_PRT(ImU32, background_colour, IM_COL32(80, 80, 80, 200));
    GX_GET_VAL_PRT(ImU32, toggled_background_colour, background_colour);
    GX_GET_VAL_PRT(ImU32, border_colour, IM_COL32(128, 128, 128, 200));

    void show();
    void compute_values();

public:
    explicit Button();
    virtual ~Button();
    virtual void update() = 0;
};
}

#endif