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
    : entity_builder(e.get_world()->create_shared_builder(core::sync::EndCaller(end_caller)))
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    physics::Transformation transform;
    auto& plt_app = e.get_platform_application().get_base();
    auto& cfg = plt_app.get_configuration().get_render_configuration();
    const auto entity_id = builder.get_id();
    const auto resolution_cfg_listener = cfg.get_runtime_resolution().add_observer(
        [entity_id, e = &e](const Resolution&) noexcept -> bool {
            // TODO: move this somewhere that has access to weak pointer of builder and the entity id
            auto* const c = e->get_world()->get_component<Camera>(entity_id);
            if (nullptr == c)
                return true;
            // TODO: create the new target and assign it to the camera, again u need a new query or builder pointer lock
            return true;
        });
    Camera cam(e, name, resolution_cfg_listener, end_caller);
    cam.set_view(math::Mat4x4<float>(transform.get_inverted_global_matrix()));
    std::array<math::Vec3<double>, 8> frustum_points;
    cam.generate_frustum_points(
        transform.get_global_location(),
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

gearoenix::core::ecs::entity_id_t gearoenix::render::camera::Builder::get_id() const noexcept
{
    return entity_builder->get_builder().get_id();
}
