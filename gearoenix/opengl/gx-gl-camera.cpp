#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::BloomData::BloomData(
    std::shared_ptr<Texture2D>&& horizontal_texture,
    std::shared_ptr<Texture2D>&& vertical_texture,
    std::shared_ptr<Target>&& source_target,
    std::shared_ptr<Target>&& prefilter_target,
    Targets&& horizontal_targets,
    TargetsLess&& vertical_targets,
    Targets&& upsampler_targets) noexcept
    : horizontal_texture(std::move(horizontal_texture))
    , vertical_texture(std::move(vertical_texture))
    , source_target(std::move(source_target))
    , prefilter_target(std::move(prefilter_target))
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
        std::dynamic_pointer_cast<Texture2D>(bloom_data->get_horizontal_texture()),
        std::dynamic_pointer_cast<Texture2D>(bloom_data->get_vertical_texture()),
        std::dynamic_pointer_cast<Target>(bloom_data->get_source_target()),
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
        },
        {
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[0]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[1]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[2]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[3]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[4]),
            std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[5]),
        });
}

gearoenix::gl::Camera::Camera(std::string&& name, const render::camera::Camera& c) noexcept
    : core::ecs::Component(this, std::move(name))
    , target(std::dynamic_pointer_cast<Target>(c.get_target()))
    , bloom_data(BloomData::construct(c.get_bloom_data()))
{
}

void gearoenix::gl::Camera::disable_bloom() noexcept
{
    bloom_data = std::nullopt;
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

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::gl::CameraManager::build(const std::string& name, core::sync::EndCaller&& end_caller) noexcept
{
    return std::shared_ptr<render::camera::Builder>(new CameraBuilder(dynamic_cast<Engine&>(e), name, std::move(end_caller)));
}

gearoenix::gl::CameraManager::CameraManager(Engine& e) noexcept
    : render::camera::Manager(e)
{
    core::ecs::Component::register_type<Camera>();
}

gearoenix::gl::CameraManager::~CameraManager() noexcept = default;

#endif
