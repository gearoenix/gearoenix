#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::BloomData::BloomData(
    std::shared_ptr<Target>&& prefilter_target,
    Targets&& horizontal_targets,
    Targets&& vertical_targets,
    UpTargets&& upsampler_targets) noexcept
    : prefilter_target(std::move(prefilter_target))
    , horizontal_targets(std::move(horizontal_targets))
    , vertical_targets(std::move(vertical_targets))
    , upsampler_targets(std::move(upsampler_targets))
{
}

gearoenix::gl::BloomData::~BloomData() noexcept = default;

std::optional<gearoenix::gl::BloomData> gearoenix::gl::BloomData::construct(
    const std::optional<render::camera::BloomData>& bloom_data) noexcept
{
    if (!bloom_data.has_value())
        return std::nullopt;
    return BloomData(
        std::dynamic_pointer_cast<Target>(bloom_data->get_prefilter_target()),
        {
            std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[0]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[1]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[2]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[3]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[4]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[5]),
        },
        {
            std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[0]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[1]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[2]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[3]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[4]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[5]),
        },
        {
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[0]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[1]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[2]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[3]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[4]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[5]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[6]),
        });
}

gearoenix::gl::Camera::Camera(std::string&& name, const render::camera::Camera& c) noexcept
    : core::ecs::Component(this, std::move(name))
    , target(std::dynamic_pointer_cast<Target>(c.get_target()))
    , second_target(std::dynamic_pointer_cast<Target>(c.get_second_target()))
    , bloom_data(BloomData::construct(c.get_bloom_data()))
{
}

void gearoenix::gl::Camera::disable_bloom() noexcept
{
    bloom_data = std::nullopt;
}

void gearoenix::gl::Camera::update(const render::camera::Camera& c) noexcept
{
    target = std::dynamic_pointer_cast<Target>(c.get_target());
    second_target = std::dynamic_pointer_cast<Target>(c.get_second_target());
    bloom_data = BloomData::construct(c.get_bloom_data());
}

gearoenix::gl::Camera::~Camera() noexcept = default;

gearoenix::gl::Camera::Camera(Camera&&) noexcept = default;

gearoenix::gl::CameraBuilder::CameraBuilder(Engine& e, const std::string& name, core::sync::EndCaller&& end_caller) noexcept
    : render::camera::Builder(e, name, std::move(end_caller))
    , eng(e)
{
    auto& builder = entity_builder->get_builder();
    Camera camera(name + "-gl-camera", *entity_builder->get_builder().get_component<render::camera::Camera>());
    builder.add_component(std::move(camera));
}

void gearoenix::gl::CameraBuilder::set_customised_target(std::shared_ptr<render::texture::Target>&& target) noexcept
{
    entity_builder->get_builder().get_component<Camera>()->target = std::dynamic_pointer_cast<Target>(target);
    render::camera::Builder::set_customised_target(std::move(target));
}

void gearoenix::gl::CameraBuilder::disable_bloom() noexcept
{
    Builder::disable_bloom();
    entity_builder->get_builder().get_component<Camera>()->disable_bloom();
}

gearoenix::gl::CameraBuilder::~CameraBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::gl::CameraManager::build_v(const std::string& name, core::sync::EndCaller&& end_caller) noexcept
{
    return std::shared_ptr<render::camera::Builder>(new CameraBuilder(dynamic_cast<Engine&>(e), name, std::move(end_caller)));
}

gearoenix::gl::CameraManager::CameraManager(Engine& e) noexcept
    : render::camera::Manager(e)
{
    core::ecs::Component::register_type<Camera>();
    resolution_cfg_listener_with_entity_id_v = [this](core::ecs::entity_id_t id, const render::Resolution&) noexcept -> void {
        auto [c, rnd_c] = this->e.get_world()->get_components<Camera, render::camera::Camera>(id);
        c->update(*rnd_c);
    };
    resolution_cfg_listener_with_entity_builder_v = [](render::camera::Builder& b, const render::Resolution&) noexcept -> void {
        auto* const c = b.get_entity_builder()->get_builder().get_component<Camera>();
        c->update(b.get_camera());
    };
}

void gearoenix::gl::CameraManager::window_resized() noexcept
{
    Manager::window_resized();
    e.get_world()->parallel_system<core::ecs::And<Camera, render::camera::Camera>>(
        [](const core::ecs::entity_id_t, Camera* const c, render::camera::Camera* const rnd_c, const unsigned int) noexcept -> void {
            c->update(*rnd_c);
        });
}

gearoenix::gl::CameraManager::~CameraManager() noexcept = default;

#endif
