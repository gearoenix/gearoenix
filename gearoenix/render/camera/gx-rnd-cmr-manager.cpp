#include "gx-rnd-cmr-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-cmr-camera.hpp"

gearoenix::render::camera::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

void gearoenix::render::camera::Manager::update() noexcept
{
    auto* const world = e.get_world();
    world->parallel_system<core::ecs::And<Camera, physics::Transformation, physics::collider::Frustum>>(
        [](
            const core::ecs::Entity::id_t /*entity-id*/,
            Camera* const cam,
            physics::Transformation* const transform,
            physics::collider::Frustum* const collider,
            const unsigned int /*kernel_index*/) {
            cam->set_view(math::Mat4x4<float>(transform->get_inverted_global_matrix()));
            std::array<math::Vec3<double>, 8> points;
            cam->generate_frustum_points(
                transform->get_global_location(),
                transform->get_x_axis(),
                transform->get_y_axis(),
                transform->get_z_axis(),
                points);
            collider->update(points);
        });
}

void gearoenix::render::camera::Manager::window_resized() noexcept
{
    e.get_world()->parallel_system<core::ecs::And<Camera>>([this](const core::ecs::entity_id_t, Camera* const c, const unsigned int) noexcept -> void {
        c->set_resolution_config(e.get_platform_application().get_base().get_configuration().get_render_configuration().get_runtime_resolution().get());
    });
}
