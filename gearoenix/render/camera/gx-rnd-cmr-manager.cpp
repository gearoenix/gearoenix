#include "gx-rnd-cmr-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-cmr-camera.hpp"

gearoenix::render::camera::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

void gearoenix::render::camera::Manager::update() noexcept
{
    auto* const world = e.get_world();
    world->parallel_system<Camera, physics::Transformation, physics::collider::Frustum>(
        [&](
            const core::ecs::Entity::id_t /*entity-id*/,
            Camera& cam,
            physics::Transformation& transform,
            physics::collider::Frustum& collider) {
            cam.set_view(math::Mat4x4<float>(transform.get_inverted_matrix()));
            std::array<math::Vec3<double>, 8> points;
            cam.generate_frustum_points(
                transform.get_location(),
                transform.get_x_axis(),
                transform.get_y_axis(),
                transform.get_z_axis(),
                points);
            collider.update(points);
        });
}