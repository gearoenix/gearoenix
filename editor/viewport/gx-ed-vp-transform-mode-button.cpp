#include "gx-ed-vp-transform-mode-button.hpp"
#include "../gx-editor-main.hpp"

#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-camera.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gizmo/gx-rnd-gzm-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-2d.hpp>

#include <ImGui/imgui.h>

gearoenix::editor::viewport::TransformModeButton::TransformModeButton(const Button& previous)
    : previous(previous)
{
    tooltip = "Global Transformation [Toggle Projection to Local]";
    toggled_tooltip = "Local Transformation [Toggle Projection to Global]";
    text = "Global";
    toggled_text = "  Local";
    start_point = { previous.get_end_point().x + spacing, top_margin };
    size = { height * 3.1f, height };
    compute_values();

    core::job::send_job_to_pool([this] {
        const render::texture::TextureInfo txt_info;
        render::texture::Manager::get().create_2d_from_file(
            platform::stream::Path::create_asset("icons/local-transform.png"),
            txt_info,
            core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) {
                toggled_icon = std::move(t);
            }));
        render::texture::Manager::get().create_2d_from_file(
            platform::stream::Path::create_asset("icons/global-transform.png"),
            txt_info,
            core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) {
                icon = std::move(t);
            }));
    });
}

gearoenix::editor::viewport::TransformModeButton::~TransformModeButton() = default;

void gearoenix::editor::viewport::TransformModeButton::update()
{
    show();
    if (clicked_in_this_frame) {
        if (toggled) {
            render::gizmo::Manager::get().enable_local_transform_mode();
        } else {
            render::gizmo::Manager::get().disable_local_transform_mode();
        }
    }
}
