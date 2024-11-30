#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../physics/gx-phs-transformation.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::CameraTarget::~CameraTarget() = default;

gearoenix::gl::CameraTarget gearoenix::gl::CameraTarget::construct(const render::camera::Target& target)
{
    if (target.is_default()) {
        const auto& d = target.get_default();
        CameraTarget result { .target = Default {} };
        auto& gd = std::get<DEFAULT_VAR_INDEX>(result.target);
        gd.main = std::dynamic_pointer_cast<Target>(d.main);
        for (std::uint32_t ti = 0; ti < d.targets.size(); ++ti) {
            for (std::uint32_t mi = 0; mi < d.targets[ti].size(); ++mi) {
                gd.targets[ti][mi] = std::dynamic_pointer_cast<Target>(d.targets[ti][mi]);
            }
        }
        return result;
    }
    if (target.is_customised()) {
        const auto& d = target.get_customised();
        return CameraTarget { .target = Customised { .target = std::dynamic_pointer_cast<Target>(d.target) } };
    }
    GX_UNEXPECTED;
}

const gearoenix::gl::CameraTarget::Customised& gearoenix::gl::CameraTarget::get_customised() const
{
    GX_ASSERT(CUSTOMISED_VAR_INDEX == target.index());
    return std::get<CUSTOMISED_VAR_INDEX>(target);
}

const gearoenix::gl::CameraTarget::Default& gearoenix::gl::CameraTarget::get_default() const
{
    GX_ASSERT(DEFAULT_VAR_INDEX == target.index());
    return std::get<DEFAULT_VAR_INDEX>(target);
}

void gearoenix::gl::Camera::set_customised_target(std::shared_ptr<render::texture::Target>&& t)
{
    gl_target.target = CameraTarget::Customised { .target = std::dynamic_pointer_cast<Target>(t) };
    render::camera::Camera::set_customised_target(std::move(t));
}

void gearoenix::gl::Camera::update_target(core::job::EndCaller<>&& end)
{
    render::camera::Camera::update_target(core::job::EndCaller([this, self = get_camera_self().lock(), end = std::move(end)] {
        (void)end;
        (void)self;
        gl_target = CameraTarget::construct(target);
    }));
}

gearoenix::gl::Camera::Camera(
    Engine& e,
    const std::string& name,
    render::camera::Target&& target,
    std::shared_ptr<physics::Transformation>&& transform,
    const core::ecs::entity_id_t entity_id)
    : render::camera::Camera(e, create_this_type_index(this), name, std::move(target), std::move(transform), entity_id)
{
}

void gearoenix::gl::Camera::construct(
    Engine& e, const std::string& name, core::job::EndCallerShared<Camera>&& c,
    std::shared_ptr<physics::Transformation>&& transform, const core::ecs::entity_id_t entity_id)
{
    c.set_return(Component::construct<Camera>(e, name, render::camera::Target(), std::move(transform), entity_id));
    c.get_return()->set_camera_self(c.get_return());
    c.get_return()->update_target(core::job::EndCaller([c] {
        c.get_return()->enable_bloom();
    }));
}

gearoenix::gl::Camera::~Camera() = default;

gearoenix::gl::CameraBuilder::CameraBuilder(
    Engine& e, const std::string& name, core::job::EndCaller<>&& entity_end_caller,
    physics::Transformation* const parent_transform)
    : Builder(e, name, std::move(entity_end_caller), parent_transform)
    , eng(e)
{
}

void gearoenix::gl::CameraBuilder::construct(
    Engine& e, const std::string& name,
    physics::Transformation* parent_transform,
    core::job::EndCallerShared<Builder>&& builder_end_caller,
    core::job::EndCaller<>&& entity_end_caller)
{
    builder_end_caller.set_return(std::make_shared<CameraBuilder>(
        e, name, std::move(entity_end_caller), parent_transform));
    const auto entity_id = builder_end_caller.get_return()->get_id();
    auto transform = builder_end_caller.get_return()->get_transformation_shared_ptr();
    Camera::construct(
        e, name + "-gl-camera",
        core::job::EndCallerShared<Camera>([b = std::move(builder_end_caller)](std::shared_ptr<Camera>&& c) {
            b.get_return()->get_entity_builder()->get_builder().add_component(std::move(c));
        }),
        std::move(transform),
        entity_id);
}

gearoenix::gl::CameraBuilder::~CameraBuilder() = default;

void gearoenix::gl::CameraManager::build(
    const std::string& name,
    physics::Transformation* const parent_transform,
    core::job::EndCallerShared<render::camera::Builder>&& builder_end_caller,
    core::job::EndCaller<>&& entity_end_caller)
{
    CameraBuilder::construct(dynamic_cast<Engine&>(e), name, parent_transform, std::move(builder_end_caller), std::move(entity_end_caller));
}

gearoenix::gl::CameraManager::CameraManager(Engine& e)
    : Manager(e)
{
    core::ecs::Component::register_type<Camera>();
}

void gearoenix::gl::CameraManager::window_resized()
{
    Manager::window_resized();
}

gearoenix::gl::CameraManager::~CameraManager() = default;

#endif
