#include "rnd-rfl-runtime.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-manager.hpp"
#include "../camera/rnd-cmr-perspective.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "../texture/rnd-txt-target.hpp"
#include "../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::reflection::Runtime::Runtime(const core::Id id, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Reflection(id, Type::Runtime, e)
{
    constexpr texture::Face faces[6] = {
        texture::Face::PositiveX,
        texture::Face::NegativeX,
        texture::Face::PositiveY,
        texture::Face::NegativeY,
        texture::Face::PositiveZ,
        texture::Face::NegativeZ,
    };
    constexpr texture::TextureInfo texture_info {
        .format = texture::TextureFormat::RgbaFloat32,
        .sample_info = texture::SampleInfo {
            .min_filter = texture::Filter::Linear,
            .mag_filter = texture::Filter::Linear,
            .wrap_s = texture::Wrap::ClampToEdge,
            .wrap_t = texture::Wrap::ClampToEdge,
            .wrap_r = texture::Wrap::ClampToEdge,
        },
        .texture_type = texture::Type::TextureCube,
        .has_mipmap = false,
    };
    auto* const sys_app = e->get_system_application();
    auto* const ast_mgr = sys_app->get_asset_manager();
    auto* const txt_mgr = ast_mgr->get_texture_manager();
    auto* const cam_mgr = ast_mgr->get_camera_manager();
    const auto resolution = static_cast<unsigned int>(sys_app->get_configuration().render_config.runtime_reflection_resolution);
    core::sync::EndCaller<texture::TextureCube> txt_cube_call([call](const std::shared_ptr<texture::TextureCube>&) {});
    environment = txt_mgr->create_cube(texture_info, resolution, txt_cube_call);
    for (int i = 1; i < 6; ++i) {
        auto& target = targets[i];
        target = e->create_render_target(
            core::asset::Manager::create_id(),
            { texture::AttachmentInfo {
                .texture_info = texture_info,
                .img_width = resolution,
                .img_height = resolution,
                .usage = texture::UsageFlag::Color,
                .face = faces[i],
                .txt = environment,
            } },
            call);
        auto& cam = cameras[i];
        cam = cam_mgr->create<camera::Perspective>();
        cam->set_target(target.get());
    }
}

gearoenix::render::reflection::Runtime::~Runtime() noexcept = default;
