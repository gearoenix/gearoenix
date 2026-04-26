#include "gx-ed-vp-projection-button.hpp"
#include "../gx-editor-main.hpp"
#include "gx-ed-vp-camera.hpp"
#include "gx-ed-vp-viewport.hpp"

#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-camera.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-texture-2d.hpp>

#include <ImGui/imgui.h>

gearoenix::editor::viewport::ProjectionButton::ProjectionButton()
{
    tooltip = "Toggle Projection to orthographic";
    toggled_tooltip = "Toggle Projection to perspective";
    text = "  Perspective Projection";
    toggled_text = "Orthographic Projection";

    // Background load both icons (perspective/orthographic cube). The textures become
    // available asynchronously; the `show()` draw code tolerates a null icon pointer
    // and simply draws the pill+label until the asset finishes loading.
    core::job::send_job_to_pool([this] {
        const render::texture::TextureInfo txt_info;
        render::texture::Manager::get().create_2d_from_file(
            platform::stream::Path::create_asset("icons/orthographic-cube.png"),
            txt_info,
            core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) {
                toggled_icon = std::move(t);
            }));
        render::texture::Manager::get().create_2d_from_file(
            platform::stream::Path::create_asset("icons/perspective-cube.png"),
            txt_info,
            core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) {
                icon = std::move(t);
            }));
    });
}

gearoenix::editor::viewport::ProjectionButton::~ProjectionButton() = default;

float gearoenix::editor::viewport::ProjectionButton::compute_start_x() const
{
    // First button on the row — sits at the left margin inside the 3D area.
    return spacing;
}

ImVec2 gearoenix::editor::viewport::ProjectionButton::compute_size() const
{
    // Width is 7.5× the row height, so the longest label ("<space> Perspective Projection")
    // fits comfortably even at larger UI text sizes.
    return { height * 7.5f, height };
}

void gearoenix::editor::viewport::ProjectionButton::update()
{
    show();

    if (clicked_in_this_frame) {
        // Swap the camera's current projection data with the previously stored one,
        // so the user can ping-pong between perspective / orthographic and keep the
        // FOV / orthographic box they had configured in either mode.
        auto& camera = *Viewport::get().get_camera()->get_camera();
        const auto cpd = camera.get_projection_data();
        camera.set_projection_data(previous_projection_data);
        previous_projection_data = cpd;
    }
}
