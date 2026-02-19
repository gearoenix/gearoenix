#include "gx-rnd-rfl-runtime.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../texture/gx-rnd-txt-target.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::reflection::Runtime::Runtime(
    core::ecs::Entity* const entity, const core::object_type_index_t final_component_type_index, const math::Aabb3<double>& receive_box, const math::Aabb3<double>& exclude_box, const math::Aabb3<double>& include_box, std::string&& name)
    : Probe(entity, final_component_type_index, include_box, std::move(name))
    , on_rendered([] { })
    , receive_box(receive_box)
    , exclude_box(exclude_box)
{
    GX_ASSERT(receive_box.get_center() == exclude_box.get_center());
    GX_ASSERT(include_box.get_center() == exclude_box.get_center());
}

void gearoenix::render::reflection::Runtime::set_runtime_reflection_self(const std::uint32_t environment_resolution, const std::uint32_t irradiance_resolution, const std::uint32_t radiance_resolution, core::job::EndCaller<>&& end_callback)
{
    const auto radiance_mipmap_levels = static_cast<std::uint32_t>(RuntimeConfiguration::compute_radiance_mipmaps_count(static_cast<std::uint16_t>(radiance_resolution)));
    const auto nears = math::Vec3<float>(exclude_box.get_diameter() * 0.5);
    const auto fars = math::Vec3<float>(receive_box.get_diameter() * 0.5);
    struct FaceInfo {
        texture::Face face;
        double x_rotate;
        double y_rotate;
        double z_rotate;
        float near;
        float far;
    };
    const std::array<FaceInfo, 6> faces { {
        { texture::FACES[0], std::numbers::pi * 0.0, std::numbers::pi * 1.5, std::numbers::pi * 1.0, nears.x, fars.x },
        { texture::FACES[1], std::numbers::pi * 0.0, std::numbers::pi * 0.5, std::numbers::pi * 1.0, nears.x, fars.x },
        { texture::FACES[2], std::numbers::pi * 0.5, std::numbers::pi * 0.0, std::numbers::pi * 0.0, nears.y, fars.y },
        { texture::FACES[3], std::numbers::pi * 1.5, std::numbers::pi * 0.0, std::numbers::pi * 0.0, nears.y, fars.y },
        { texture::FACES[4], std::numbers::pi * 1.0, std::numbers::pi * 0.0, std::numbers::pi * 0.0, nears.z, fars.z },
        { texture::FACES[5], std::numbers::pi * 0.0, std::numbers::pi * 0.0, std::numbers::pi * 1.0, nears.z, fars.z },
    } };
    const auto roughness_move = 1.0 / static_cast<double>(radiance_mipmap_levels - 1);
    double current_roughness = 0.0;
    for (auto i = decltype(radiance_mipmap_levels) { 0 }; i < radiance_mipmap_levels; ++i) {
        roughnesses.push_back(current_roughness);
        current_roughness += roughness_move;
    }
    const auto environment_texture_info = // ---
        texture::TextureInfo()
            .set_format(engine::Engine::get().get_specification().is_float_texture_supported ? texture::TextureFormat::RgbaFloat32 : texture::TextureFormat::RgbaUint8)
            .set_sampler_info(
                texture::SamplerInfo().set_min_filter(texture::Filter::LinearMipmapLinear).set_mag_filter(texture::Filter::Linear).set_wrap_s(texture::Wrap::ClampToEdge).set_wrap_t(texture::Wrap::ClampToEdge).set_wrap_r(texture::Wrap::ClampToEdge))
            .set_width(environment_resolution)
            .set_height(environment_resolution)
            .set_type(texture::Type::TextureCube)
            .set_has_mipmap(true);
    auto runtime_self = object_self.lock();

    const core::job::EndCaller when_all_textures_are_ready([this, _s = runtime_self, faces, main_end_callback = std::move(end_callback)]() -> void {
        for (auto face_index = 0; face_index < 6; ++face_index) {
            const auto& face = faces[face_index];
            const auto name_ext = "-" + std::to_string(face.face);
            const auto& cam = cameras[face_index].cmr;
            texture::Manager::get().create_target(object_name + "-environment-target" + name_ext,
                std::vector { texture::Attachment(texture::AttachmentCube(std::shared_ptr(environment), face.face)), texture::Attachment(texture::Attachment2D(std::shared_ptr(environment_depth))) },
                core::job::EndCallerShared<texture::Target>([this, main_end_callback, _s, face_index, cam](std::shared_ptr<texture::Target>&& t) -> void {
                    environment_targets[face_index] = std::move(t);
                    cam->set_customised_target(std::shared_ptr(environment_targets[face_index]));
                }));
            cam->set_near(face.near);
            cam->set_far(face.far);
            cam->set_projection_data(camera::ProjectionData::construct_perspective(std::numbers::pi * 0.5));
            cam->set_usage(camera::Camera::Usage::ReflectionProbe);
            cam->get_entity()->set_parent(entity);
            cam->set_enabled(false);
            const auto& transform = cameras[face_index].trn;
            transform->local_inner_x_rotate(face.x_rotate);
            transform->local_inner_y_rotate(face.y_rotate);
            transform->local_inner_z_rotate(face.z_rotate);
            transform->set_local_position(receive_box.get_center());
            texture::Manager::get().create_target(object_name + "-irradiance-target" + name_ext, std::vector { texture::Attachment(texture::AttachmentCube(std::shared_ptr(irradiance), face.face)) },
                core::job::EndCallerShared<texture::Target>([this, main_end_callback, _s, face_index, cam](std::shared_ptr<texture::Target>&& t) { irradiance_targets[face_index] = std::move(t); }));
            radiance_targets[face_index].resize(roughnesses.size());
            for (std::uint64_t mip_index = 0; mip_index < roughnesses.size(); ++mip_index) {
                texture::Manager::get().create_target(object_name + "-radiance-target" + name_ext + "-" + std::to_string(mip_index),
                    std::vector { texture::Attachment(texture::AttachmentCube(std::shared_ptr(radiance), face.face), static_cast<std::uint8_t>(mip_index)) },
                    core::job::EndCallerShared<texture::Target>([this, main_end_callback, _s, face_index, mip_index, cam](std::shared_ptr<texture::Target>&& t) { radiance_targets[face_index][mip_index] = std::move(t); }));
            }
        }
    });

    texture::Manager::get().create_cube_from_pixels(
        object_name + "-environment", {}, environment_texture_info, core::job::EndCallerShared<texture::TextureCube>([this, when_all_textures_are_ready, self = runtime_self](std::shared_ptr<texture::TextureCube>&& t) {
            environment = std::move(t);
            (void)when_all_textures_are_ready;
            (void)self;
        }));
    auto irradiance_texture_info = environment_texture_info;
    irradiance_texture_info.set_width(irradiance_resolution);
    irradiance_texture_info.set_height(irradiance_resolution);
    texture::Manager::get().create_cube_from_pixels(
        object_name + "-irradiance", {}, irradiance_texture_info, core::job::EndCallerShared<texture::TextureCube>([this, when_all_textures_are_ready, self = runtime_self](std::shared_ptr<texture::TextureCube>&& t) {
            irradiance = std::move(t);
            (void)when_all_textures_are_ready;
            (void)self;
        }));
    auto radiance_texture_info = environment_texture_info;
    radiance_texture_info.set_width(radiance_resolution);
    radiance_texture_info.set_height(radiance_resolution);
    std::vector<std::vector<std::vector<std::uint8_t>>> radiance_pixels(6);
    constexpr auto pixel_size = static_cast<std::uint64_t>(4) * static_cast<std::uint64_t>(4);
    const std::uint64_t base_mipmap_radiance_size = pixel_size * radiance_resolution * radiance_resolution;
    for (auto& fp : radiance_pixels) {
        fp.resize(roughnesses.size());
        auto psz = base_mipmap_radiance_size;
        for (auto& p : fp) {
            p.resize(psz);
            std::memset(p.data(), 0, psz);
            psz >>= 2;
        }
    }
    texture::Manager::get().create_cube_from_pixels(
        object_name + "-radiance", {}, radiance_texture_info, core::job::EndCallerShared<texture::TextureCube>([this, when_all_textures_are_ready, self = runtime_self](std::shared_ptr<texture::TextureCube>&& t) {
            set_radiance(std::move(t));
            (void)when_all_textures_are_ready;
            (void)self;
        }));
    texture::Manager::get().create_2d_from_pixels(object_name + "-environment-depth", {},
        texture::TextureInfo()
            .set_format(texture::TextureFormat::D32)
            .set_sampler_info(
                texture::SamplerInfo().set_min_filter(texture::Filter::Nearest).set_mag_filter(texture::Filter::Nearest).set_wrap_s(texture::Wrap::ClampToEdge).set_wrap_t(texture::Wrap::ClampToEdge).set_wrap_r(texture::Wrap::ClampToEdge))
            .set_width(environment_resolution)
            .set_height(environment_resolution)
            .set_type(texture::Type::Texture2D)
            .set_has_mipmap(false),
        core::job::EndCallerShared<texture::Texture2D>([this, when_all_textures_are_ready, self = runtime_self](std::shared_ptr<texture::Texture2D>&& t) {
            environment_depth = std::move(t);
            (void)when_all_textures_are_ready;
            (void)self;
        }));
    for (std::uint64_t face_index = 0; face_index < 6; ++face_index) {
        const auto& face = faces[face_index];
        const auto name_ext = "-" + std::to_string(face.face);
        auto camera_name = std::string(object_name).append("-camera").append(name_ext);
        camera::Manager::get().build(std::move(camera_name), entity, core::job::EndCaller<core::ecs::EntityPtr>([this, face_index, when_all_textures_are_ready, self = runtime_self](core::ecs::EntityPtr&& camera_entity) {
            cameras[face_index].cmr = camera_entity->get_component_shared_ptr<camera::Camera>();
            cameras[face_index].trn = camera_entity->get_component_shared_ptr<physics::Transformation>();
            (void)when_all_textures_are_ready;
            (void)self;
        }));
    }
}

gearoenix::render::reflection::Runtime::~Runtime() = default;

void gearoenix::render::reflection::Runtime::set_location(const math::Vec3<double>& p)
{
    receive_box.set_center(p);
    exclude_box.set_center(p);
    include_box.set_center(p);
    for (const auto& camera_data : cameras) {
        camera_data.trn->set_local_position(p);
    }
}

void gearoenix::render::reflection::Runtime::update_state()
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
        cameras[0].cmr->set_enabled(true);
        break;
    case State::EnvironmentCubeRender:
        cameras[state_environment_face].cmr->set_enabled(false);
        ++state_environment_face;
        if (state_environment_face < 6) {
            cameras[state_environment_face].cmr->set_enabled(true);
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

void gearoenix::render::reflection::Runtime::set_on_rendered(std::function<void()>&& f) { on_rendered = std::move(f); }

void gearoenix::render::reflection::Runtime::export_baked(const std::shared_ptr<platform::stream::Stream>& s, core::job::EndCaller<>&& end_callback) const
{
    include_box.write(*s);
    irradiance->write(s, core::job::EndCaller([s, radiance = radiance, end_callback = std::move(end_callback)] { radiance->write(s, end_callback); }));
}
