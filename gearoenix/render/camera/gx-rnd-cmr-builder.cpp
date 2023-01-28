#include "gx-rnd-cmr-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "gx-rnd-cmr-camera.hpp"

gearoenix::render::camera::Builder::Builder(engine::Engine& e, const std::string& name, core::sync::EndCaller&& end_caller) noexcept
    : entity_builder(e.get_world()->create_shared_builder(std::string(name), core::sync::EndCaller(end_caller)))
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    physics::Transformation transform(name + "-transformation");
    auto& plt_app = e.get_platform_application().get_base();
    auto& cfg = plt_app.get_configuration().get_render_configuration();
    const auto entity_id = builder.get_id();
    Camera cam(e, name, 0, end_caller);
    cam.set_view(math::Mat4x4<float>(transform.get_inverted_global_matrix()));
    std::array<math::Vec3<double>, 8> frustum_points;
    cam.generate_frustum_points(
        transform.get_global_location(),
        transform.get_x_axis(),
        transform.get_y_axis(),
        transform.get_z_axis(),
        frustum_points);
    builder.add_component(physics::collider::Frustum(name + "-collider", frustum_points));
    builder.add_component(std::move(cam));
    builder.add_component(std::move(transform));
}

gearoenix::render::camera::Builder::~Builder() noexcept = default;

gearoenix::physics::Transformation& gearoenix::render::camera::Builder::get_transformation() noexcept
{
    return *entity_builder->get_builder().get_component<physics::Transformation>();
}

const gearoenix::physics::Transformation& gearoenix::render::camera::Builder::get_transformation() const noexcept
{
    return *entity_builder->get_builder().get_component<physics::Transformation>();
}

gearoenix::render::camera::Camera& gearoenix::render::camera::Builder::get_camera() noexcept
{
    return *entity_builder->get_builder().get_component<Camera>();
}

const gearoenix::render::camera::Camera& gearoenix::render::camera::Builder::get_camera() const noexcept
{
    return *entity_builder->get_builder().get_component<Camera>();
}

void gearoenix::render::camera::Builder::set_customised_target(std::shared_ptr<texture::Target>&& target) noexcept
{
    entity_builder->get_builder().get_component<Camera>()->set_customised_target(std::move(target));
}

void gearoenix::render::camera::Builder::disable_bloom() noexcept
{
    entity_builder->get_builder().get_component<Camera>()->disable_bloom();
}

gearoenix::core::ecs::entity_id_t gearoenix::render::camera::Builder::get_id() const noexcept
{
    return entity_builder->get_builder().get_id();
}
