#include "rnd-rfl-runtime.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-manager.hpp"
#include "../camera/rnd-cmr-perspective.hpp"
#include "../camera/rnd-cmr-transformation.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "../texture/rnd-txt-target.hpp"
#include "../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::reflection::Runtime::Runtime(
    const core::Id id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Reflection(id, Type::Runtime, e)
{
    constexpr std::tuple<texture::Face, math::Vec3, math::Vec3> faces[6] = {
        { texture::FACES[0], math::Vec3(1.0f, 0.0f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f) },
        { texture::FACES[1], math::Vec3(-1.0f, 0.0f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f) },
        { texture::FACES[2], math::Vec3(0.0f, 1.0f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f) },
        { texture::FACES[3], math::Vec3(0.0f, -1.0f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f) },
        { texture::FACES[4], math::Vec3(0.0f, 0.0f, 1.0f), math::Vec3(0.0f, -1.0f, 0.0f) },
        { texture::FACES[5], math::Vec3(0.0f, 0.0f, -1.0f), math::Vec3(0.0f, 1.0f, 0.0f) },
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
    for (int i = 0; i < 6; ++i) {
        auto& target = environment_targets[i];
        target = e->create_render_target(
            core::asset::Manager::create_id(),
            { texture::AttachmentInfo {
                .texture_info = texture_info,
                .img_width = resolution,
                .img_height = resolution,
                .usage = texture::UsageFlag::Color,
                .face = std::get<0>(faces[i]),
                .txt = environment,
            } },
            call);
        auto& cam = cameras[i];
        cam = cam_mgr->create<camera::Perspective>();
        cam->set_cascaded_shadow_frustum_partitions_count(1);
        cam->set_field_of_view(1.570796327f);
        cam->set_aspects(resolution, resolution);
        cam->set_target(target.get());
        cam->get_transformation()->look_at(std::get<1>(faces[i]), std::get<2>(faces[i]));
    }
    set_receiving_radius(receiving_radius);
}

gearoenix::render::reflection::Runtime::~Runtime() noexcept = default;

void gearoenix::render::reflection::Runtime::set_receiving_radius(const core::Real r) noexcept
{
    receiving_radius = r;
    for (const auto& cam : cameras) {
        cam->set_far(r);
    }
}