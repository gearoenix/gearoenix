#include "gx-rnd-rfl-runtime.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../system/gx-sys-app.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../camera/gx-rnd-cmr-perspective.hpp"
#include "../camera/gx-rnd-cmr-transformation.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../graph/node/gx-rnd-gr-nd-irradiance-convoluter.hpp"
#include "../graph/node/gx-rnd-gr-nd-mipmap-generator.hpp"
#include "../graph/node/gx-rnd-gr-nd-radiance-convoluter.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-target.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::reflection::Runtime::Runtime(
    const core::Id id,
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end_call) noexcept
    : Reflection(id, std::move(name), Type::Runtime, e)
    , on_rendered([] {})
{
    core::sync::EndCaller<core::sync::EndCallerIgnore> call([end_call, this] {
        state = State::Started;
    });
    constexpr std::tuple<texture::Face, math::Vec3<double>, math::Vec3<double>> faces[6] = {
        { texture::FACES[0], math::Vec3(1.0, 0.0, 0.0), math::Vec3(0.0, -1.0, 0.0) },
        { texture::FACES[1], math::Vec3(-1.0, 0.0, 0.0), math::Vec3(0.0, -1.0, 0.0) },
        { texture::FACES[2], math::Vec3(0.0, 1.0, 0.0), math::Vec3(0.0, 0.0, 1.0) },
        { texture::FACES[3], math::Vec3(0.0, -1.0, 0.0), math::Vec3(0.0, 0.0, -1.0) },
        { texture::FACES[4], math::Vec3(0.0, 0.0, 1.0), math::Vec3(0.0, -1.0, 0.0) },
        { texture::FACES[5], math::Vec3(0.0, 0.0, -1.0), math::Vec3(0.0, -1.0, 0.0) },
    };
    constexpr texture::TextureInfo texture_info {
        .format = texture::TextureFormat::RgbaFloat32,
        .sampler_info = texture::SamplerInfo {
            .min_filter = texture::Filter::LinearMipmapLinear,
            .mag_filter = texture::Filter::Linear,
            .wrap_s = texture::Wrap::ClampToEdge,
            .wrap_t = texture::Wrap::ClampToEdge,
            .wrap_r = texture::Wrap::ClampToEdge,
        },
        .texture_type = texture::Type::TextureCube,
        .has_mipmap = true,
    };
    auto* const sys_app = e->get_system_application();
    auto* const ast_mgr = sys_app->get_asset_manager();
    auto* const txt_mgr = ast_mgr->get_texture_manager();
    auto* const cam_mgr = ast_mgr->get_camera_manager();
    auto* const msh_mgr = ast_mgr->get_mesh_manager();
    const auto& render_config = sys_app->get_configuration().render_config;
    const auto environment_resolution = static_cast<unsigned int>(render_config.get_runtime_reflection_environment_resolution());
    const auto irradiance_resolution = static_cast<unsigned int>(render_config.get_runtime_reflection_irradiance_resolution());
    const auto radiance_resolution = static_cast<unsigned int>(render_config.get_runtime_reflection_radiance_resolution());
    const auto radiance_mips_count = render_config.get_runtime_reflection_radiance_levels();
    for (auto i = 0; i < radiance_mips_count; ++i) {
        const auto fi = static_cast<double>(i);
        const auto fm = static_cast<double>(radiance_mips_count - 1);
        roughnesses.push_back(fi / fm);
    }
#ifdef GX_DEBUG_MODE
    if ((radiance_resolution >> radiance_mips_count) < 2)
        GXLOGF("Unexpected resolution and levels for radiance convolution in render config.")
#endif
    core::sync::EndCaller<texture::TextureCube> txt_cube_call([call](const std::shared_ptr<texture::TextureCube>&) {});
    core::sync::EndCaller<mesh::Mesh> msh_call([call](const std::shared_ptr<mesh::Mesh>&) {});
    environment = txt_mgr->create_cube(
        "runtime-reflection-" + this->name + "-environment-texture",
        texture_info, environment_resolution, txt_cube_call);
    environment_mipmap_generator = std::make_shared<graph::node::MipmapGenerator>(
        "runtime-reflection-" + this->name + "-environment-mipmap-generator", environment.get(), e, call);
    irradiance = txt_mgr->create_cube(
        "runtime-reflection-" + this->name + "-environment-irradiance",
        texture_info, irradiance_resolution, txt_cube_call);
    irradiance_mipmap_generator = std::make_shared<graph::node::MipmapGenerator>(
        "runtime-reflection-" + this->name + "-irradiance-mipmap-generator", irradiance.get(), e, call);
    radiance = txt_mgr->create_cube(
        "runtime-reflection-" + this->name + "-radiance-texture",
        texture_info, radiance_resolution, txt_cube_call);
    std::vector<texture::AttachmentInfo> target_infos = { texture::AttachmentInfo {
        .texture_info = texture_info,
        .usage = texture::UsageFlag::Color,
    } };
    auto& target_info = target_infos[0];
    for (int i = 0; i < 6; ++i) {
        target_info.face = std::get<0>(faces[i]);
        target_info.mipmap_level = 0;
        // environment related stuff
        target_info.img_width = target_info.img_height = environment_resolution;
        target_info.txt = environment;
        auto& target = environment_targets[i];
        target = e->create_render_target(core::asset::Manager::create_id(), target_infos, call);
        auto& cam = cameras[i];
        cam = cam_mgr->create<camera::Perspective>("runtime-reflection-" + this->name + "-camera");
        cam->set_cascaded_shadow_frustum_partitions_count(1);
        cam->set_field_of_view(1.570796327f);
        cam->set_aspects(environment_resolution, environment_resolution);
        cam->set_target(target.get());
        cam->get_transformation()->look_at(std::get<1>(faces[i]), std::get<2>(faces[i]));
        cam->set_gamma_correction_factor(0.0f);
        // mesh
        auto& face_mesh = face_meshes[i];
        face_mesh = msh_mgr->create_face_square(texture::FACES[i], msh_call);
        // irradiance related stuff
        auto& irradiance_target = irradiance_targets[i];
        target_info.img_width = target_info.img_height = irradiance_resolution;
        target_info.txt = irradiance;
        irradiance_target = e->create_render_target(core::asset::Manager::create_id(), target_infos, call);
        auto& irradiance_convoluter = irradiance_convoluters[i];
        const auto mvp = math::Mat4x4<float>::perspective(1.0f, 1.0f, 0.5, 2.0f) * math::Mat4x4<float>::look_at(math::Vec3(0.0f), math::Vec3<float>(std::get<1>(faces[i])), math::Vec3<float>(std::get<2>(faces[i])));
        irradiance_convoluter = std::make_unique<graph::node::IrradianceConvoluter>(
            "runtime-reflection-" + this->name + "-irradiance-convoluter-" + std::to_string(i),
            mvp, environment.get(), e, call);
        irradiance_convoluter->set_render_target(irradiance_target.get());
        // radiance part
        auto& radiance_face_targets = radiance_targets[i];
        radiance_face_targets.resize(roughnesses.size());
        auto& radiance_face_convoluters = radiance_convoluters[i];
        radiance_face_convoluters.resize(roughnesses.size());
        target_info.txt = radiance;
        for (std::size_t mi = 0; mi < roughnesses.size(); ++mi) {
            target_info.img_width = target_info.img_height = static_cast<unsigned int>(radiance->get_aspect() >> mi);
            target_info.mipmap_level = static_cast<unsigned int>(mi);
            auto& radiance_target = radiance_face_targets[mi];
            radiance_target = e->create_render_target(core::asset::Manager::create_id(), target_infos, call);
            auto& radiance_convoluter = radiance_face_convoluters[mi];
            radiance_convoluter = std::make_unique<graph::node::RadianceConvoluter>(
                "runtime-reflection-" + this->name + "-radiance-convoluter-" + std::to_string(i) + "-" + std::to_string(mi),
                static_cast<float>(roughnesses[mi]), static_cast<float>(environment_resolution),
                face_mesh.get(), environment.get(), e, call);
            radiance_convoluter->set_render_target(radiance_target.get());
        }
    }
    set_maximum_receiving_radius(maximum_receiving_radius);
    set_minimum_receiving_radius(minimum_receiving_radius);
}

gearoenix::render::reflection::Runtime::~Runtime() noexcept = default;

void gearoenix::render::reflection::Runtime::set_maximum_receiving_radius(const double r) noexcept
{
    maximum_receiving_radius = r;
    for (const auto& cam : cameras) {
        cam->set_far(static_cast<float>(r));
    }
}

void gearoenix::render::reflection::Runtime::set_minimum_receiving_radius(const double r) noexcept
{
    minimum_receiving_radius = r;
    for (const auto& cam : cameras) {
        cam->set_near(static_cast<float>(r));
    }
}

// physics::Transformation implementation

void gearoenix::render::reflection::Runtime::set_location(const math::Vec3<double>& p) noexcept
{
    for (const auto& cam : cameras) {
        cam->get_transformation()->set_location(p);
    }
}

void gearoenix::render::reflection::Runtime::local_scale(const double s) noexcept
{
    minimum_receiving_radius *= s;
    maximum_receiving_radius *= s;
    set_maximum_receiving_radius(maximum_receiving_radius);
    set_minimum_receiving_radius(minimum_receiving_radius);
}

void gearoenix::render::reflection::Runtime::update_state() noexcept
{
    // Note this happens when already all the necessary action done for the state
    switch (state) {
    case State::Started:
        state = State::EnvironmentCubeRender;
        break;
    case State::EnvironmentCubeRender:
        ++state_environment_face;
        if (state_environment_face > 5) {
            state_environment_face = 0;
            state = State::EnvironmentCubeMipMap;
        }
        break;
    case State::EnvironmentCubeMipMap:
        state = State::IrradianceFace;
        break;
    case State::IrradianceFace:
        ++state_irradiance_face;
        if (state_irradiance_face > 5) {
            state_irradiance_face = 0;
            state = State::IrradianceMipMap;
        }
        break;
    case State::IrradianceMipMap:
        state = State::RadianceFaceLevel;
        break;
    case State::RadianceFaceLevel:
        ++state_radiance_level;
        if (state_radiance_level >= radiance_convoluters[0].size()) {
            state_radiance_level = 0;
            ++state_radiance_face;
            if (state_radiance_face > 5) {
                state_radiance_face = 0;
                state = State::Resting;
                on_rendered();
            }
        }
        break;
    case State::Resting:
        if (pending_to_start) {
            pending_to_start = false;
            state_resting_frame = 0;
            state = State::EnvironmentCubeRender;
        } else {
            ++state_resting_frame;
            if (state_resting_frame > resting_frames_count) {
                state_resting_frame = 0;
                state = State::EnvironmentCubeRender;
            }
        }
        break;
    default:
        break;
    }
}
