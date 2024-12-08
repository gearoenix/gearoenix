#include "gx-ed-vp-projection-button.hpp"
#include "../gx-editor-main.hpp"
#include "gx-ed-vp-viewport.hpp"
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-camera.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-2d.hpp>
#include <imgui/imgui.h>

#include "gx-ed-vp-camera.hpp"

gearoenix::editor::viewport::ProjectionButton::ProjectionButton(Application& app)
    : app(app)
{
    tooltip = "Toggle Projection to orthographic";
    toggled_tooltip = "Toggle Projection to perspective";
    text = "  Perspective Projection";
    toggled_text = "Orthographic Projection";
    start_point = { spacing, top_margin };
    size = { height * 7.5f, height };
    compute_values();

    core::job::send_job_to_pool([this] {
        auto& txt_mgr = *this->app.get_render_engine().get_texture_manager();
        const render::texture::TextureInfo txt_info;
        txt_mgr.create_2d_from_file(
            platform::stream::Path::create_asset("icons/orthographic-cube.png"),
            txt_info,
            core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) {
                toggled_icon = std::move(t);
            }));
        txt_mgr.create_2d_from_file(
            platform::stream::Path::create_asset("icons/perspective-cube.png"),
            txt_info,
            core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) {
                icon = std::move(t);
            }));
    });
}

gearoenix::editor::viewport::ProjectionButton::~ProjectionButton() = default;

void gearoenix::editor::viewport::ProjectionButton::update()
{
    show();

    if (clicked_in_this_frame) {
        auto& camera = *app.get_viewport()->get_camera()->get_camera();
        const auto cpd = camera.get_projection_data();
        camera.set_projection_data(previous_projection_data);
        previous_projection_data = cpd;
    }
}
