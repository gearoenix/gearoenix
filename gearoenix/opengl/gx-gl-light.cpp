#include "gx-gl-light.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

namespace {
const auto shadow_caster_directional_allocator = gearoenix::core::allocator::SharedArray<gearoenix::gl::ShadowCasterDirectionalLight, gearoenix::render::light::ShadowCasterDirectional::MAX_COUNT>::construct();
}

const boost::container::flat_set<std::type_index>& gearoenix::gl::ShadowCasterDirectionalLight::get_all_the_hierarchy_types_except_component() const
{
    static const boost::container::flat_set types {
        create_type_index<Light>(),
        create_type_index<ShadowCasterDirectional>(),
        create_this_type_index(this)
    };
    return types;
}

gearoenix::gl::ShadowCasterDirectionalLight::ShadowCasterDirectionalLight(std::string&& name)
    : ShadowCasterDirectional(create_this_type_index(this), std::move(name))
{
}

std::shared_ptr<gearoenix::gl::ShadowCasterDirectionalLight> gearoenix::gl::ShadowCasterDirectionalLight::construct(
    std::string&& name)
{
    auto self = shadow_caster_directional_allocator->make_shared(std::move(name));
    self->set_component_self(self);
    return self;
}

gearoenix::gl::ShadowCasterDirectionalLight::~ShadowCasterDirectionalLight() = default;

void gearoenix::gl::ShadowCasterDirectionalLight::set_shadow_map(
    std::shared_ptr<render::texture::Texture2D>&& t,
    core::job::EndCaller<>&& end_callback)
{
    shadow_map_texture = std::dynamic_pointer_cast<Texture2D>(t);
    shadow_map_texture_v = shadow_map_texture->get_object();
    ShadowCasterDirectional::set_shadow_map(std::move(t), std::move(end_callback));
}

void gearoenix::gl::ShadowCasterDirectionalLight::set_shadow_map_target(std::shared_ptr<render::texture::Target>&& t)
{
    shadow_map_target = std::dynamic_pointer_cast<Target>(t);
    shadow_map_target_v = shadow_map_target->get_framebuffer();
    ShadowCasterDirectional::set_shadow_map_target(std::move(t));
}

gearoenix::gl::LightBuilder::LightBuilder(
    Engine& e,
    const std::string& name,
    physics::TransformationComponent* const parent_transform,
    const ShadowCasterDirectionalInfo& info,
    core::job::EndCaller<>&& end_callback)
    : Builder(e, name, parent_transform, info, std::move(end_callback))
    , eng(e)
{
    auto& b = entity_builder->get_builder();
    b.add_component(ShadowCasterDirectionalLight::construct(name + "-gl-directional-shadow-caster"));
}

gearoenix::gl::LightBuilder::LightBuilder(
    Engine& e,
    const std::string& name,
    physics::TransformationComponent* const parent_transform,
    const DirectionalInfo& info,
    core::job::EndCaller<>&& end_callback)
    : Builder(e, name, parent_transform, info, std::move(end_callback))
    , eng(e)
{
}

void gearoenix::gl::LightBuilder::construct(
    Engine& e,
    const std::string& name,
    physics::TransformationComponent* const parent_transform,
    const ShadowCasterDirectionalInfo& info,
    core::job::EndCallerShared<Builder>&& end_callback,
    core::job::EndCaller<>&& entity_end_callback)
{
    auto builder = std::shared_ptr<LightBuilder>(new LightBuilder(e, name, parent_transform, info, std::move(entity_end_callback)));
    end_callback.set_return(std::shared_ptr(builder));
    builder->get_shadow_caster_directional()->initialise(
        e, parent_transform, info.shadow_map_resolution, info.far, info.near, info.aspect, builder,
        core::job::EndCaller([e = std::move(end_callback)] {}));
}

gearoenix::gl::LightBuilder::~LightBuilder() = default;

std::shared_ptr<gearoenix::render::light::Builder> gearoenix::gl::LightManager::build_directional(
    const std::string& name,
    physics::TransformationComponent* const parent_transform,
    core::job::EndCaller<>&& end_callback)
{
    return std::make_shared<LightBuilder>(
        eng, name, parent_transform, render::light::Builder::DirectionalInfo {}, std::move(end_callback));
}

void gearoenix::gl::LightManager::build_shadow_caster_directional(
    const std::string& name,
    physics::TransformationComponent* const parent_transform,
    const std::uint32_t shadow_map_resolution,
    const float camera_far,
    const float camera_near,
    const float camera_aspect,
    core::job::EndCallerShared<render::light::Builder>&& end_callback,
    core::job::EndCaller<>&& entity_end_callback)
{
    return LightBuilder::construct(
        eng, name, parent_transform,
        render::light::Builder::ShadowCasterDirectionalInfo {
            .shadow_map_resolution = shadow_map_resolution,
            .far = camera_far,
            .near = camera_near,
            .aspect = camera_aspect,
        },
        std::move(end_callback), std::move(entity_end_callback));
}

gearoenix::gl::LightManager::LightManager(Engine& e)
    : Manager(e)
    , eng(e)
{
}

gearoenix::gl::LightManager::~LightManager() = default;

#endif
