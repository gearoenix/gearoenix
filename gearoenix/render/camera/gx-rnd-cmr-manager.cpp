#include "gx-rnd-cmr-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-cmr-builder.hpp"
#include "gx-rnd-cmr-camera.hpp"

gearoenix::render::camera::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
    core::ecs::Component::register_type<Camera>();
}

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::render::camera::Manager::build(
    const std::string& name, core::sync::EndCaller&& end_caller) noexcept
{
    auto builder = build_v(name, std::move(end_caller));
    builder->get_camera().resolution_cfg_listener = e.get_platform_application().get_base().get_configuration().get_render_configuration().get_runtime_resolution().add_observer(
        [entity_id = builder->get_id(), this, builder = std::weak_ptr<Builder>(builder), failures_count = 0](const Resolution& r) mutable noexcept -> bool {
            auto* c = e.get_world()->get_component<Camera>(entity_id);
            if (nullptr == c) {
                if (const auto b = builder.lock(); nullptr != b) {
                    b->get_camera().update_target();
                    resolution_cfg_listener_with_entity_builder_v(*b, r);
                } else if (++failures_count > 100) {
                    return false;
                }
            } else {
                c->update_target();
                resolution_cfg_listener_with_entity_id_v(entity_id, r);
            }
            return true;
        });
    return builder;
}

void gearoenix::render::camera::Manager::update() noexcept
{
    auto* const world = e.get_world();
    world->parallel_system<core::ecs::And<Camera, physics::Transformation, physics::collider::Frustum>>(
        [](
            const core::ecs::entity_id_t /*entity-id*/,
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
    e.get_world()->parallel_system<core::ecs::And<Camera>>([](const core::ecs::entity_id_t, Camera* const c, const unsigned int) noexcept -> void {
        c->update_target();
    });
}
