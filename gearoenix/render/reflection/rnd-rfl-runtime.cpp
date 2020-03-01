#include "rnd-rfl-runtime.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-manager.hpp"
#include "../camera/rnd-cmr-perspective.hpp"
#include "../camera/rnd-cmr-transformation.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../graph/node/rnd-gr-nd-irradiance-convoluter.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
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
    auto* const msh_mgr = ast_mgr->get_mesh_manager();
    const auto environment_resolution = static_cast<unsigned int>(sys_app->get_configuration().render_config.runtime_reflection_environment_resolution);
    const auto diffuse_resolution = static_cast<unsigned int>(sys_app->get_configuration().render_config.runtime_reflection_diffuse_resolution);
    core::sync::EndCaller<texture::TextureCube> txt_cube_call([call](const std::shared_ptr<texture::TextureCube>&) {});
    core::sync::EndCaller<mesh::Mesh> msh_call([call](const std::shared_ptr<mesh::Mesh>&) {});
    environment = txt_mgr->create_cube(texture_info, environment_resolution, txt_cube_call);
    diffuse = txt_mgr->create_cube(texture_info, diffuse_resolution, txt_cube_call);
    std::vector<texture::AttachmentInfo> target_infos = { texture::AttachmentInfo {
        .texture_info = texture_info,
        .usage = texture::UsageFlag::Color,
    } };
    for (int i = 0; i < 6; ++i) {
        target_infos[0].face = std::get<0>(faces[i]);
        // environment related stuff
        target_infos[0].img_width = environment_resolution;
        target_infos[0].img_height = environment_resolution;
        target_infos[0].txt = environment;
        auto& target = environment_targets[i];
        target = e->create_render_target(core::asset::Manager::create_id(), target_infos, call);
        auto& cam = cameras[i];
        cam = cam_mgr->create<camera::Perspective>();
        cam->set_cascaded_shadow_frustum_partitions_count(1);
        cam->set_field_of_view(1.570796327f);
        cam->set_aspects(environment_resolution, environment_resolution);
        cam->set_target(target.get());
        cam->get_transformation()->look_at(std::get<1>(faces[i]), std::get<2>(faces[i]));
        // mesh
        auto& face_mesh = face_meshes[i];
        face_mesh = msh_mgr->create_face_square(texture::FACES[i], msh_call);
        // diffuse related stuff
        auto& diffuse_target = diffuse_targets[i];
        target_infos[0].img_width = diffuse_resolution;
        target_infos[0].img_height = diffuse_resolution;
        target_infos[0].txt = diffuse;
        diffuse_target = e->create_render_target(core::asset::Manager::create_id(), target_infos, call);
        auto& irradiance_convoluter = irradiance_convoluters[i];
        irradiance_convoluter = std::make_unique<graph::node::IrradianceConvoluter>(
            face_mesh.get(), environment.get(), e, call);
        irradiance_convoluter->set_render_target(diffuse_target.get());
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