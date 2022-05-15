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
#include "gx-rnd-rfl-builder.hpp"
#include <tuple>

gearoenix::render::reflection::Runtime::Runtime(
    engine::Engine& e,
    Builder& builder,
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
    , receive_box(receive_box)
    , exclude_box(exclude_box)
    , include_box(include_box)
{
    GX_ASSERT(receive_box.get_center() == exclude_box.get_center());
    GX_ASSERT(include_box.get_center() == exclude_box.get_center());
    GX_ASSERT((radiance_resolution >> radiance_mipmap_levels) > 2);
    const auto nears = math::Vec3<float>(exclude_box.get_diameter() * 0.5);
    const auto fars = math::Vec3<float>(receive_box.get_diameter() * 0.5);
    const std::tuple<texture::Face, double /*x-rotate*/, double /*y-rotate*/, double /*z-rotate*/, float /*near*/, float /*far*/> faces[6] = {
        { texture::FACES[0], GX_PI * 0.0, GX_PI * 1.5, GX_PI * 1.0, nears.x, fars.x },
        { texture::FACES[1], GX_PI * 0.0, GX_PI * 0.5, GX_PI * 1.0, nears.x, fars.x },
        { texture::FACES[2], GX_PI * 0.5, GX_PI * 0.0, GX_PI * 0.0, nears.y, fars.y },
        { texture::FACES[3], GX_PI * 1.5, GX_PI * 0.0, GX_PI * 0.0, nears.y, fars.y },
        { texture::FACES[4], GX_PI * 1.0, GX_PI * 0.0, GX_PI * 0.0, nears.z, fars.z },
        { texture::FACES[5], GX_PI * 0.0, GX_PI * 0.0, GX_PI * 1.0, nears.z, fars.z },
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
    environment = txt_mgr->create_cube_from_pixels(name + "-environment", {}, environment_texture_info, end_callback);
    auto irradiance_texture_info = environment_texture_info;
    irradiance_texture_info.width = irradiance_resolution;
    irradiance_texture_info.height = irradiance_resolution;
    irradiance = txt_mgr->create_cube_from_pixels(name + "-irradiance", {}, irradiance_texture_info, end_callback);
    auto radiance_texture_info = environment_texture_info;
    radiance_texture_info.width = radiance_resolution;
    radiance_texture_info.height = radiance_resolution;
    std::vector<std::vector<std::vector<std::uint8_t>>> radiance_pixels(6);
    const std::size_t pixel_size = 4 * 4;
    const std::size_t base_mipmap_radiance_size = pixel_size * radiance_resolution * radiance_resolution;
    for (auto& fp : radiance_pixels) {
        fp.resize(roughnesses.size());
        auto psz = base_mipmap_radiance_size;
        for (auto& p : fp) {
            p.resize(psz);
            std::memset(p.data(), 0, psz);
            psz >>= 2;
        }
    }
    radiance = txt_mgr->create_cube_from_pixels(name + "-radiance", {}, radiance_texture_info, end_callback);
    auto env_depth = txt_mgr->create_2d_from_pixels(
        name + "-environment-depth",
        {},
        texture::TextureInfo {
            .format = texture::TextureFormat::D32,
            .sampler_info = texture::SamplerInfo {
                .min_filter = texture::Filter::Nearest,
                .mag_filter = texture::Filter::Nearest,
                .wrap_s = texture::Wrap::ClampToEdge,
                .wrap_t = texture::Wrap::ClampToEdge,
                .wrap_r = texture::Wrap::ClampToEdge,
            },
            .width = environment_resolution,
            .height = environment_resolution,
            .type = texture::Type::Texture2D,
            .has_mipmap = false,
        },
        end_callback);
    for (std::size_t face_index = 0; face_index < 6; ++face_index) {
        const auto& face = faces[face_index];
        const auto name_ext = "-" + std::to_string(std::get<0>(face));
        auto camera_builder = cam_mgr->build(name + "-camera" + name_ext);
        cameras[face_index] = camera_builder->get_entity_builder()->get_builder().get_id();
        environment_targets[face_index] = txt_mgr->create_target(
            name + "-environment-target" + name_ext,
            std::vector<texture::Attachment> {
                texture::Attachment {
                    .var = texture::AttachmentCube {
                        .txt = environment,
                        .face = std::get<0>(face),
                    } },
                texture::Attachment { .var = texture::Attachment2D {
                                          .txt = env_depth,
                                      } },
            },
            end_callback);
        camera_builder->set_target(std::shared_ptr<texture::Target>(environment_targets[face_index]));
        auto& cam = camera_builder->get_camera();
        cam.set_yfov(static_cast<float>(GX_PI * 0.25));
        cam.set_target_aspect_ratio(1.0f);
        cam.set_near(std::get<4>(face));
        cam.set_far(std::get<5>(face));
        cam.set_usage(camera::Camera::Usage::ReflectionProbe);
        cam.set_reference_id(builder.get_entity_builder()->get_builder().get_id());
        cam.enabled = false;
        auto& transform = camera_builder->get_transformation();
        transform.local_x_rotate(std::get<1>(face));
        transform.local_y_rotate(std::get<2>(face));
        transform.local_z_rotate(std::get<3>(face));
        transform.set_location(receive_box.get_center());
        builder.set_camera_builder(std::move(camera_builder), face_index);
        irradiance_targets[face_index] = txt_mgr->create_target(
            name + "-irradiance-target" + name_ext,
            std::vector<texture::Attachment> {
                texture::Attachment {
                    .var = texture::AttachmentCube {
                        .txt = irradiance,
                        .face = std::get<0>(face),
                    } } },
            end_callback);
        auto& radiance_face_targets = radiance_targets[face_index];
        radiance_face_targets.resize(roughnesses.size());
        for (std::size_t mip_index = 0; mip_index < roughnesses.size(); ++mip_index) {
            radiance_face_targets[mip_index] = txt_mgr->create_target(
                name + "-radiance-target" + name_ext + "-" + std::to_string(mip_index),
                std::vector<texture::Attachment> {
                    texture::Attachment {
                        .mipmap_level = static_cast<unsigned int>(mip_index),
                        .var = texture::AttachmentCube {
                            .txt = radiance,
                            .face = std::get<0>(face),
                        } } },
                end_callback);
        }
    }
}

gearoenix::render::reflection::Runtime::~Runtime() noexcept = default;

gearoenix::render::reflection::Runtime::Runtime(Runtime&& o) noexcept = default;

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
    case State::Uninitialized:
        ++state_resting_frame;
        if (state_resting_frame > 10) {
            state_resting_frame = 0;
            state = State::Started;
        }
        break;
    case State::Started:
        state = State::EnvironmentCubeRender;
        state_environment_face = 0;
        e.get_world()->get_component<camera::Camera>(cameras[0])->enabled = true;
        break;
    case State::EnvironmentCubeRender:
        e.get_world()->get_component<camera::Camera>(cameras[state_environment_face])->enabled = false;
        ++state_environment_face;
        if (state_environment_face < 6) {
            e.get_world()->get_component<camera::Camera>(cameras[state_environment_face])->enabled = true;
        } else {
            state_environment_face = 0;
            state = State::EnvironmentCubeMipMap;
        }
        break;
    case State::EnvironmentCubeMipMap:
        state = State::IrradianceFace;
        state_irradiance_face = 0;
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
        state_radiance_level = 0;
        state_radiance_face = 0;
        break;
    case State::RadianceFaceLevel:
        ++state_radiance_level;
        if (state_radiance_level >= roughnesses.size()) {
            state_radiance_level = 0;
            ++state_radiance_face;
            if (state_radiance_face > 5) {
                state_radiance_face = 0;
                state = State::Resting;
                if (on_rendered.has_value())
                    (*on_rendered)();
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

void gearoenix::render::reflection::Runtime::set_scene_id(const core::ecs::Entity::id_t si) noexcept
{
    scene_id = si;
    for (const auto camera_id : cameras)
        if (auto* const c = e.get_world()->get_component<camera::Camera>(camera_id); nullptr != c)
            c->set_scene_id(scene_id);
}
