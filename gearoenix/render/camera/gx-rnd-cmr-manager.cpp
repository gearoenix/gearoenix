#include "gx-rnd-cmr-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "gx-rnd-cmr-builder.hpp"
#include "gx-rnd-cmr-camera.hpp"

gearoenix::render::camera::Manager::Manager(engine::Engine& e)
    : e(e)
{
    core::ecs::ComponentType::add<Camera>();
}

void gearoenix::render::camera::Manager::update()
{
    core::ecs::World::get()->parallel_system<core::ecs::All<Camera, physics::Transformation, physics::collider::Frustum>>(
        [](
            const core::ecs::entity_id_t /*entity-id*/,
            Camera* const cam,
            const physics::Transformation* const transform,
            physics::collider::Frustum* const collider,
            const unsigned int /*kernel_index*/) {
            cam->set_view(math::Mat4x4<float>(transform->get_inverted_global_matrix()));
            std::array<math::Vec3<double>, 8> points;
            cam->generate_frustum_points(
                transform->get_global_position(),
                transform->get_x_axis(),
                transform->get_y_axis(),
                transform->get_z_axis(),
                points);
            collider->update(points);
        });
}

void gearoenix::render::camera::Manager::window_resized()
{
    core::ecs::World::get()->parallel_system<core::ecs::All<Camera>>([](const core::ecs::entity_id_t, Camera* const c, const unsigned int) -> void {
        c->update_target(core::job::EndCaller([] { }));
    });
}
