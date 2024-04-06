#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

namespace {
gearoenix::core::allocator::SharedArray<gearoenix::gl::Camera, gearoenix::render::camera::Camera::MAX_COUNT> allocator;
}

gearoenix::gl::BloomData::BloomData(
        std::shared_ptr<Target> &&prefilter_target,
        Targets &&horizontal_targets,
        Targets &&vertical_targets,
        UpTargets &&upsampler_targets)
    : prefilter_target(std::move(prefilter_target))
    , horizontal_targets(std::move(horizontal_targets))
    , vertical_targets(std::move(vertical_targets))
    , upsampler_targets(std::move(upsampler_targets))
{
}

gearoenix::gl::BloomData::~BloomData() = default;

std::optional<gearoenix::gl::BloomData> gearoenix::gl::BloomData::construct(const std::optional<render::camera::BloomData> &bloom_data) {
    if(!bloom_data.has_value()) { return std::nullopt; }
    return BloomData(
            std::dynamic_pointer_cast<Target>(bloom_data->get_prefilter_target()),
            {
                    std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[0]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[1]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[2]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[3]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[4]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_horizontal_targets()[5]),
            },
            {
                    std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[0]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[1]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[2]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[3]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[4]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_vertical_targets()[5]),
            },
            {
                    std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[0]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[1]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[2]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[3]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[4]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[5]),
                    std::dynamic_pointer_cast<Target>(bloom_data->get_upsampler_targets()[6]),
            });
}

gearoenix::gl::CameraTarget::~CameraTarget() = default;

gearoenix::gl::CameraTarget gearoenix::gl::CameraTarget::construct(const render::camera::Target &target) noexcept {
    if(target.is_default()) {
        const auto& d = target.get_default();
        return CameraTarget {
            .target = Default {
                    .first = std::dynamic_pointer_cast<Target>(d.first),
                    .second = std::dynamic_pointer_cast<Target>(d.second),
            }
        };
    }
    if(target.is_customised()) {
        const auto& d = target.get_customised();
        return CameraTarget { .target = Customised { .target = std::dynamic_pointer_cast<Target>(d.target) } };
    }
    GX_UNEXPECTED;
}

const gearoenix::gl::CameraTarget::Customised &gearoenix::gl::CameraTarget::get_customised() const {
    GX_ASSERT(CUSTOMISED_VAR_INDEX == target.index());
    return std::get<CUSTOMISED_VAR_INDEX>(target);
}

const gearoenix::gl::CameraTarget::Default &gearoenix::gl::CameraTarget::get_default() const {
    GX_ASSERT(DEFAULT_VAR_INDEX == target.index());
    return std::get<DEFAULT_VAR_INDEX>(target);
}

const boost::container::flat_set<std::type_index>& gearoenix::gl::Camera::get_all_the_hierarchy_types_except_component() const
{
    static const boost::container::flat_set types { create_type_index<render::camera::Camera>(), create_this_type_index(this) };
    return types;
}

void gearoenix::gl::Camera::set_customised_target(std::shared_ptr<render::texture::Target>&& t)
{
    gl_target.target = CameraTarget::Customised { .target = std::dynamic_pointer_cast<Target>(t) };
    render::camera::Camera::set_customised_target(std::move(t));
}

void gearoenix::gl::Camera::disable_bloom() {
    render::camera::Camera::disable_bloom();
    gl_bloom_data = std::nullopt;
}

void gearoenix::gl::Camera::enable_bloom(core::job::EndCaller<> &&end) {
    render::camera::Camera::enable_bloom(core::job::EndCaller([this, self = get_camera_self().lock(), end = std::move(end)]{
        (void) end;
        (void) self;
        gl_bloom_data = BloomData::construct(bloom_data);
    }));
}

void gearoenix::gl::Camera::update_target(gearoenix::core::job::EndCaller<> &&end) {
    render::camera::Camera::update_target(core::job::EndCaller([this, self = get_camera_self().lock(), end = std::move(end)]{
        (void) end;
        (void) self;
        gl_target = CameraTarget::construct(target);
    }));
}

gearoenix::gl::Camera::Camera(Engine& e, const std::string& name, render::camera::Target&& target)
    : render::camera::Camera(e, create_this_type_index(this), name, std::move(target))
{
}

void gearoenix::gl::Camera::construct(Engine& e, const std::string& name, core::job::EndCallerShared<Camera>&& c)
{
    c.set_return(allocator.make_shared(e, name, render::camera::Target()));
    c.get_return()->set_component_self(c.get_return());
    c.get_return()->update_target(core::job::EndCaller([c] {
        c.get_return()->enable_bloom(core::job::EndCaller([c] {
            (void) c;
        }));
    }));
}

gearoenix::gl::Camera::~Camera() = default;

gearoenix::gl::CameraBuilder::CameraBuilder(Engine& e, const std::string& name, core::job::EndCaller<>&& end_caller)
    : Builder(e, name, std::move(end_caller))
    , eng(e)
{
}

void gearoenix::gl::CameraBuilder::construct(
        Engine &e, const std::string &name,
        core::job::EndCallerShared<render::camera::Builder> &&builder_end_caller,
        core::job::EndCaller<> &&entity_end_caller) {
    builder_end_caller.set_return(std::make_shared<CameraBuilder>(e, name, std::move(entity_end_caller)));
    Camera::construct(
            e, name + "-gl-camera",
            core::job::EndCallerShared<Camera>([b = std::move(builder_end_caller)](std::shared_ptr<Camera>&& c) {
                b.get_return()->get_entity_builder()->get_builder().add_component(std::move(c));
            }));
}

gearoenix::gl::CameraBuilder::~CameraBuilder() = default;

void gearoenix::gl::CameraManager::build(
    const std::string& name,
    core::job::EndCallerShared<render::camera::Builder>&& builder_end_caller,
    core::job::EndCaller<>&& entity_end_caller)
{
    CameraBuilder::construct(dynamic_cast<Engine&>(e), name, std::move(builder_end_caller), std::move(entity_end_caller));
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
