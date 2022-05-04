#include "gx-rnd-rfl-runtime.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-target.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include <tuple>

gearoenix::render::reflection::Runtime::Runtime(
    engine::Engine& e,
    const std::string& name,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::size_t environment_resolution,
    const std::size_t irradiance_resolution,
    const std::size_t radiance_resolution,
    const std::size_t radiance_mipmap_levels,
    const core::sync::EndCallerIgnored& end_callback) noexcept
    : core::ecs::Component(this)
    , e(e)
    , on_rendered([] {})
    , receive_box(receive_box)
    , exclude_box(exclude_box)
    , include_box(include_box)
{
    GX_ASSERT(receive_box.get_center() == exclude_box.get_center());
    GX_ASSERT(include_box.get_center() == exclude_box.get_center());
    GX_ASSERT((radiance_resolution >> radiance_mipmap_levels) < 2);
    core::sync::EndCallerIgnored call([end_callback, this] {
        state = State::Started;
    });
    const auto nears = math::Vec3<float>(exclude_box.get_diameter() * 0.5);
    const auto fars = math::Vec3<float>(receive_box.get_diameter() * 0.5);
    const std::tuple<texture::Face, math::Vec3<double> /*target*/, math::Vec3<double> /*up*/, float /*near*/, float /*far*/> faces[6] = {
        { texture::FACES[0], math::Vec3(1.0, 0.0, 0.0), math::Vec3(0.0, -1.0, 0.0), nears.x, fars.x },
        { texture::FACES[1], math::Vec3(-1.0, 0.0, 0.0), math::Vec3(0.0, -1.0, 0.0), nears.x, fars.x },
        { texture::FACES[2], math::Vec3(0.0, 1.0, 0.0), math::Vec3(0.0, 0.0, 1.0), nears.y, fars.y },
        { texture::FACES[3], math::Vec3(0.0, -1.0, 0.0), math::Vec3(0.0, 0.0, -1.0), nears.y, fars.y },
        { texture::FACES[4], math::Vec3(0.0, 0.0, 1.0), math::Vec3(0.0, -1.0, 0.0), nears.z, fars.z },
        { texture::FACES[5], math::Vec3(0.0, 0.0, -1.0), math::Vec3(0.0, -1.0, 0.0), nears.z, fars.z },
    };
    auto* const txt_mgr = e.get_texture_manager();
    auto* const cam_mgr = e.get_camera_manager();
    const auto roughness_move = 1.0 / static_cast<double>(radiance_mipmap_levels - 1);
    double current_roughness = 0.0;
    for (auto i = decltype(radiance_mipmap_levels) { 0 }; i < radiance_mipmap_levels; ++i, current_roughness += roughness_move) {
        roughnesses.push_back(current_roughness);
    }
    const texture::TextureInfo environment_texture_info {
        .format = texture::TextureFormat::RgbaFloat32,
        .sampler_info = texture::SamplerInfo {
            .min_filter = texture::Filter::Linear,
            .mag_filter = texture::Filter::Linear,
            .wrap_s = texture::Wrap::ClampToEdge,
            .wrap_t = texture::Wrap::ClampToEdge,
            .wrap_r = texture::Wrap::ClampToEdge,
        },
        .width = environment_resolution,
        .height = environment_resolution,
        .type = texture::Type::TextureCube,
        .has_mipmap = true,
    };
    environment = txt_mgr->create_cube_from_pixels(name + "-environment", {}, environment_texture_info, call);
    auto irradiance_texture_info = environment_texture_info;
    irradiance_texture_info.width = irradiance_resolution;
    irradiance_texture_info.height = irradiance_resolution;
    irradiance = txt_mgr->create_cube_from_pixels(name + "-irradiance", {}, irradiance_texture_info, call);
    auto radiance_texture_info = environment_texture_info;
    radiance_texture_info.width = radiance_resolution;
    radiance_texture_info.height = radiance_resolution;
    radiance = txt_mgr->create_cube_from_pixels(name + "-radiance", {}, radiance_texture_info, call);
    for (std::size_t face_index = 0; face_index < 6; ++face_index) {
        const auto& face = faces[face_index];
        const auto name_ext = "-" + std::to_string(std::get<0>(face));
        auto camera_builder = cam_mgr->build(name + "-camera" + name_ext);
        camera_builder->set_target(txt_mgr->create_target(
            name + "-target" + name_ext,
            std::vector<texture::Attachment> {
                texture::Attachment {
                    .var = texture::AttachmentCube {
                        .txt = environment,
                        .face = std::get<0>(face),
                    } } },
            end_callback));
        auto& cam = camera_builder->get_camera();
        cam.set_yfov(1.570796327f);
        cam.set_target_aspect_ratio(1.0f);
        cam.set_near(std::get<3>(face));
        cam.set_far(std::get<4>(face));
        auto& transform = camera_builder->get_transformation();
        transform.look_at(std::get<1>(face), std::get<2>(face));
        transform.set_location(receive_box.get_center());
    }
}

gearoenix::render::reflection::Runtime::~Runtime() noexcept = default;

gearoenix::render::reflection::Runtime::Runtime(Runtime&&) noexcept = default;

void gearoenix::render::reflection::Runtime::set_location(const math::Vec3<double>& p) noexcept
{
    receive_box.set_center(p);
    exclude_box.set_center(p);
    include_box.set_center(p);
    for (const auto id : cameras) {
        auto* const trn = e.get_world()->get_component<physics::Transformation>(id);
        trn->set_location(p);
    }
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
        if (state_radiance_level >= roughnesses.size()) {
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
