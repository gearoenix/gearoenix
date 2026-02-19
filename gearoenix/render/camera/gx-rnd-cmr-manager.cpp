#include "gx-rnd-cmr-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "gx-rnd-cmr-camera.hpp"

gearoenix::render::camera::Manager::Manager()
    : Singleton(this)
{
    core::ecs::ComponentType::add<Camera>();
}

void gearoenix::render::camera::Manager::build_impl(std::string&& name, core::ecs::Entity* const parent, const core::job::EndCaller<core::ecs::EntityPtr>& entity_callback)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    auto transform = core::Object::construct<physics::Transformation>(entity.get(), entity->get_object_name() + "-transformation");
    auto frustum = core::Object::construct<physics::collider::Frustum>(entity.get(), std::shared_ptr(transform), entity->get_object_name() + "-collider", physics::collider::Frustum::default_points);
    entity->add_component(std::move(frustum));
    entity->add_component(std::move(transform));
    entity_callback.set_return(std::move(entity));
}

gearoenix::render::camera::Manager::~Manager() = default;

void gearoenix::render::camera::Manager::build(std::string&&, core::ecs::Entity*, core::job::EndCaller<core::ecs::EntityPtr>&&)
{
    GX_UNEXPECTED; // graphic backend must implement this.
}

void gearoenix::render::camera::Manager::update()
{
    core::ecs::World::get().parallel_system<core::ecs::All<Camera, physics::Transformation, physics::collider::Frustum>>([](const auto /*entity-id*/, auto* const cam, const auto* const transform, auto* const collider, const auto /*kernel_index*/) {
        cam->set_view(math::Mat4x4<float>(transform->get_transposed_inverted_global_matrix().transposed()));
        std::array<math::Vec3<double>, 8> points;
        cam->generate_frustum_points(transform->get_global_position(), transform->get_x_axis(), transform->get_y_axis(), transform->get_z_axis(), points);
        collider->update(points);
    });
}

void gearoenix::render::camera::Manager::window_resized()
{
    core::ecs::World::get().parallel_system<core::ecs::All<Camera>>([](const auto, auto* const c, const auto) -> void { c->update_target(core::job::EndCaller([] { })); });
}
