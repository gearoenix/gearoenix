#include "gx-ed-vp-scale-button.hpp"
#include "../gx-editor-main.hpp"

#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gizmo/gx-rnd-gzm-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-2d.hpp>

#include <ImGui/imgui.h>

gearoenix::editor::viewport::ScaleButton::ScaleButton(const Button& previous)
    : previous(previous)
{
    tooltip = "Enable scale gizmo";
    toggled_tooltip = "Disable scale gizmo";
    text = "";
    toggled_text = "";
    start_point = { previous.get_end_point().x + spacing, top_margin };
    size = { height, height };
    toggled_background_colour = IM_COL32(90, 90, 200, 200);
    compute_values();

    core::job::send_job_to_pool([this] {
        const render::texture::TextureInfo txt_info;
        render::texture::Manager::get().create_2d_from_file(
            platform::stream::Path::create_asset("icons/scale.png"),
            txt_info,
            core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) {
                toggled_icon = std::move(t);
                icon = toggled_icon;
            }));
    });
}

gearoenix::editor::viewport::ScaleButton::~ScaleButton() = default;

void gearoenix::editor::viewport::ScaleButton::update()
{
    show();
    if (clicked_in_this_frame) {
        if (toggled) {
            render::gizmo::Manager::get().enable_scale_handle();
        } else {
            render::gizmo::Manager::get().disable_scale_handle();
        }
    }
}
