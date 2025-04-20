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
        auto& gd = std::get<default_var_index>(result.target);
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
    GX_ASSERT(customised_var_index == target.index());
    return std::get<customised_var_index>(target);
}

const gearoenix::gl::CameraTarget::Default& gearoenix::gl::CameraTarget::get_default() const
{
    GX_ASSERT(default_var_index == target.index());
    return std::get<default_var_index>(target);
}

void gearoenix::gl::Camera::set_customised_target(std::shared_ptr<render::texture::Target>&& t)
{
    gl_target.target = CameraTarget::Customised { .target = std::dynamic_pointer_cast<Target>(t) };
    render::camera::Camera::set_customised_target(std::move(t));
}

void gearoenix::gl::Camera::update_target(core::job::EndCaller<>&& end)
{
    render::camera::Camera::update_target(core::job::EndCaller([this, self = object_self.lock(), end = std::move(end)] {
        (void)end;
        (void)self;
        gl_target = CameraTarget::construct(target);
    }));
}

gearoenix::gl::Camera::Camera(const std::string& name, render::camera::Target&& target, std::shared_ptr<physics::Transformation>&& transform)
    : render::camera::Camera(core::ecs::ComponentType::create_index(this), name, std::move(target), std::move(transform))
{
}

void gearoenix::gl::Camera::construct(const std::string& name, core::job::EndCallerShared<Camera>&& c, std::shared_ptr<physics::Transformation>&& transform)
{
    c.set_return(Object::construct<Camera>(name, render::camera::Target(), std::move(transform)));
    c.get_return()->initialise();
    c.get_return()->update_target(core::job::EndCaller([c] {
        c.get_return()->enable_bloom();
    }));
}

gearoenix::gl::Camera::~Camera() = default;

void gearoenix::gl::CameraManager::build(
    std::string&& name,
    core::ecs::Entity* const parent,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    build_impl(std::move(name), parent, entity_callback);
    const auto* const entity = entity_callback.get_return().get();
    auto transform = entity->get_component_shared_ptr<physics::Transformation>();
    Camera::construct(
        entity->get_object_name() + "-gl-camera",
        core::job::EndCallerShared<Camera>([end = std::move(entity_callback)](std::shared_ptr<Camera>&& c) {
            end.get_return()->add_component(std::move(c));
        }),
        std::move(transform));
}

gearoenix::gl::CameraManager::CameraManager()
{
    core::ecs::ComponentType::add<Camera>();
}

void gearoenix::gl::CameraManager::window_resized()
{
    Manager::window_resized();
}

gearoenix::gl::CameraManager::~CameraManager() = default;

#endif
