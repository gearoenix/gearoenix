#include "rnd-rfl-runtime.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-manager.hpp"
#include "../camera/rnd-cmr-perspective.hpp"
#include "../camera/rnd-cmr-transformation.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../graph/node/rnd-gr-nd-irradiance-convoluter.hpp"
#include "../graph/node/rnd-gr-nd-radiance-convoluter.hpp"
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
    const auto& render_config = sys_app->get_configuration().render_config;
    const auto environment_resolution = static_cast<unsigned int>(render_config.runtime_reflection_environment_resolution);
    const auto diffuse_resolution = static_cast<unsigned int>(render_config.runtime_reflection_diffuse_resolution);
    const auto specular_resolution = static_cast<unsigned int>(render_config.runtime_reflection_specular_resolution);
    const auto specular_mips_count = render_config.runtime_reflection_specular_levels;
    for (auto i = specular_mips_count; i > 0; --i) {
        const auto fi = static_cast<core::Real>(i);
        const auto fm = static_cast<core::Real>(specular_mips_count);
        roughnesses.push_back((fi + (fm * 0.5f - fi) * 0.001f) / fm);
    }
#ifdef GX_DEBUG_MODE
    if ((specular_resolution >> specular_mips_count) < 2)
        GXLOGF("Unexpected resolution and levels for specular convolution in render config.")
#endif
    core::sync::EndCaller<texture::TextureCube> txt_cube_call([call](const std::shared_ptr<texture::TextureCube>&) {});
    core::sync::EndCaller<mesh::Mesh> msh_call([call](const std::shared_ptr<mesh::Mesh>&) {});
    environment = txt_mgr->create_cube(texture_info, environment_resolution, txt_cube_call);
    diffuse = txt_mgr->create_cube(texture_info, diffuse_resolution, txt_cube_call);
    auto specular_txt_info = texture_info;
    specular_txt_info.sample_info.min_filter = texture::Filter::LinearMipmapLinear;
    specular_txt_info.has_mipmap = true;
    specular = txt_mgr->create_cube(specular_txt_info, specular_resolution, txt_cube_call);
    std::vector<texture::AttachmentInfo> target_infos = { texture::AttachmentInfo {
        .texture_info = texture_info,
        .usage = texture::UsageFlag::Color,
    } };
    auto& target_info = target_infos[0];
    for (int i = 0; i < 6; ++i) {
        target_info.face = std::get<0>(faces[i]);
        // environment related stuff
        target_info.img_width = target_info.img_height = environment_resolution;
        target_info.txt = environment;
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
        target_info.img_width = target_info.img_height = diffuse_resolution;
        target_info.txt = diffuse;
        diffuse_target = e->create_render_target(core::asset::Manager::create_id(), target_infos, call);
        auto& irradiance_convoluter = irradiance_convoluters[i];
        irradiance_convoluter = std::make_unique<graph::node::IrradianceConvoluter>(
            face_mesh.get(), environment.get(), e, call);
        irradiance_convoluter->set_render_target(diffuse_target.get());
        // specular part
        auto& specular_face_targets = specular_targets[i];
        specular_face_targets.resize(roughnesses.size());
        auto& radiance_face_convoluters = radiance_convoluters[i];
        radiance_face_convoluters.resize(roughnesses.size());
        target_info.txt = specular;
        for (std::size_t mi = 0; mi < roughnesses.size(); ++mi) {
            target_info.img_width = target_info.img_height = specular->get_aspect() >> mi;
            target_info.mipmap_level = mi;
            auto& specular_target = specular_face_targets[mi];
            specular_target = e->create_render_target(core::asset::Manager::create_id(), target_infos, call);
            auto& radiance_convoluter = radiance_face_convoluters[mi];
            radiance_convoluter = std::make_unique<graph::node::RadianceConvoluter>(
                roughnesses[mi], face_mesh.get(), environment.get(), e, call);
            radiance_convoluter->set_render_target(specular_target.get());
        }
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