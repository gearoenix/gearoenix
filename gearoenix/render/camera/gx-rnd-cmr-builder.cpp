#include "gx-rnd-cmr-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-cmr-camera.hpp"

gearoenix::render::camera::Builder::Builder(engine::Engine& e, const std::string& name) noexcept
    : entity_builder(e.get_world()->create_shared_builder())
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    physics::Transformation transform;
    transform.local_z_translate(2.0);
    const auto& plt_app = e.get_platform_application().get_base();
    Camera cam(static_cast<float>(plt_app.get_window_width()) / static_cast<float>(plt_app.get_window_height()));
    cam.set_view(math::Mat4x4<float>(transform.get_matrix().inverted()));
    std::array<math::Vec3<double>, 8> frustum_points;
    cam.generate_frustum_points(
        transform.get_location(),
        transform.get_x_axis(),
        transform.get_y_axis(),
        transform.get_z_axis(),
        frustum_points);
    builder.add_component(physics::collider::Frustum(frustum_points));
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
