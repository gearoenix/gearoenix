#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

namespace {
const auto allocator = gearoenix::core::allocator::SharedArray<gearoenix::gl::Camera, gearoenix::render::camera::Camera::MAX_COUNT>::construct();
}

gearoenix::gl::CameraTarget::~CameraTarget() = default;

gearoenix::gl::CameraTarget gearoenix::gl::CameraTarget::construct(const render::camera::Target& target)
{
    if (target.is_default()) {
        const auto& d = target.get_default();
        CameraTarget result { .target = Default {} };
        auto& gd = std::get<DEFAULT_VAR_INDEX>(result.target);
        gd.main = std::dynamic_pointer_cast<Target>(d.main);
        for (std::size_t ti = 0; ti < d.targets.size(); ++ti) {
            for (std::size_t mi = 0; mi < d.targets[ti].size(); ++mi) {
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

const gearoenix::core::ecs::Component::HierarchyTypes& gearoenix::gl::Camera::get_hierarchy_types() const
{
    static const auto types = generate_hierarchy_types<render::camera::Camera>(this);
    return types;
}

void gearoenix::gl::Camera::set_customised_target(std::shared_ptr<render::texture::Target>&& t)
{
    gl_target.target = CameraTarget::Customised { .target = std::dynamic_pointer_cast<Target>(t) };
    render::camera::Camera::set_customised_target(std::move(t));
}

void gearoenix::gl::Camera::update_target(gearoenix::core::job::EndCaller<>&& end)
{
    render::camera::Camera::update_target(core::job::EndCaller([this, self = get_camera_self().lock(), end = std::move(end)] {
        (void)end;
        (void)self;
        gl_target = CameraTarget::construct(target);
    }));
}

gearoenix::gl::Camera::Camera(Engine& e, const std::string& name, render::camera::Target&& target)
    : render::camera::Camera(e, create_this_type_index(this), name, std::move(target))
{
}

void gearoenix::gl::Camera::construct(Engine& e, const std::string& name, core::job::EndCallerShared<Camera>&& c)
{
    c.set_return(allocator->make_shared(e, name, render::camera::Target()));
    c.get_return()->set_component_self(c.get_return());
    c.get_return()->update_target(core::job::EndCaller([c] {
        c.get_return()->enable_bloom();
    }));
}

gearoenix::gl::Camera::~Camera() = default;

gearoenix::gl::CameraBuilder::CameraBuilder(
    Engine& e, const std::string& name, core::job::EndCaller<>&& entity_end_caller,
    physics::TransformationComponent* const parent_transform)
    : Builder(e, name, std::move(entity_end_caller), parent_transform)
    , eng(e)
{
}

void gearoenix::gl::CameraBuilder::construct(
    Engine& e, const std::string& name,
    physics::TransformationComponent* parent_transform,
    core::job::EndCallerShared<Builder>&& builder_end_caller,
    core::job::EndCaller<>&& entity_end_caller)
{
    builder_end_caller.set_return(std::make_shared<CameraBuilder>(
        e, name, std::move(entity_end_caller), parent_transform));
    Camera::construct(
        e, name + "-gl-camera",
        core::job::EndCallerShared<Camera>([b = std::move(builder_end_caller)](std::shared_ptr<Camera>&& c) {
            b.get_return()->get_entity_builder()->get_builder().add_component(std::move(c));
        }));
}

gearoenix::gl::CameraBuilder::~CameraBuilder() = default;

void gearoenix::gl::CameraManager::build(
    const std::string& name,
    physics::TransformationComponent* const parent_transform,
    core::job::EndCallerShared<render::camera::Builder>&& builder_end_caller,
    core::job::EndCaller<>&& entity_end_caller)
{
    CameraBuilder::construct(dynamic_cast<Engine&>(e), name, parent_transform, std::move(builder_end_caller), std::move(entity_end_caller));
}

gearoenix::gl::CameraManager::CameraManager(Engine& e)
    : Manager(e)
{
}

void gearoenix::gl::CameraManager::window_resized()
{
    Manager::window_resized();
}

gearoenix::gl::CameraManager::~CameraManager() = default;

#endif
