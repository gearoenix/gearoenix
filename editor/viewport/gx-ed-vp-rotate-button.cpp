#include "gx-ed-vp-rotate-button.hpp"
#include "../gx-editor-main.hpp"
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-2d.hpp>
#include <imgui/imgui.h>

gearoenix::editor::viewport::RotateButton::RotateButton(Application& app, const Button& previous)
    : app(app)
    , previous(previous)
{
    tooltip = "Enable rotation gizmo";
    toggled_tooltip = "Disable rotation gizmo";
    text = "";
    toggled_text = "";
    start_point = { previous.get_end_point().x + spacing, top_margin };
    size = { height, height };
    toggled_background_colour = IM_COL32(90, 90, 200, 128);
    compute_values();

    core::job::send_job_io1([this] {
        auto& txt_mgr = *this->app.get_render_engine().get_texture_manager();
        const render::texture::TextureInfo txt_info;
        txt_mgr.create_2d_from_file(
            platform::stream::Path::create_asset("icons/rotate.png"),
            txt_info,
            core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) {
                toggled_icon = std::move(t);
                icon = toggled_icon;
            }));
    });
}

gearoenix::editor::viewport::RotateButton::~RotateButton() = default;

void gearoenix::editor::viewport::RotateButton::update()
{
    show();
}
