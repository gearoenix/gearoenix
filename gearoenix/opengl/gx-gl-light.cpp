#include "gx-gl-light.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

void gearoenix::gl::ShadowCasterDirectionalLight::write_in_io_context(
    std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) const
{
    GX_UNIMPLEMENTED;
}

gearoenix::gl::ShadowCasterDirectionalLight::ShadowCasterDirectionalLight(std::string&& name)
    : ShadowCasterDirectional(core::ecs::ComponentType::create_index(this), std::move(name))
{
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

gearoenix::core::ecs::EntityPtr gearoenix::gl::LightManager::build_directional(std::string&& name, core::ecs::Entity* const parent)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    entity->add_component(core::Object::construct<render::light::Directional>(entity->get_object_name() + "-directional-light"));
    return entity;
}

void gearoenix::gl::LightManager::build_shadow_caster_directional(
    std::string&& name,
    core::ecs::Entity* const parent,
    const std::uint32_t shadow_map_resolution,
    const float camera_far,
    const float camera_near,
    const float camera_aspect,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    entity->add_component(core::Object::construct<ShadowCasterDirectionalLight>(entity->get_object_name() + "-gl-directional-shadow-caster"));
    const auto* const ptr = entity.get();
    entity_callback.set_return(std::move(entity));
    ptr->get_component_shared_ptr<ShadowCasterDirectionalLight>()->initialise(
        shadow_map_resolution, camera_far, camera_near, camera_aspect,
        core::job::EndCaller([e = std::move(entity_callback)] { }));
}

gearoenix::gl::LightManager::LightManager()
{
    core::ecs::ComponentType::add<ShadowCasterDirectionalLight>();
}

gearoenix::gl::LightManager::~LightManager() = default;

#endif
