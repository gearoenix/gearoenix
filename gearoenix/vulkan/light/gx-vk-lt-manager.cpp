#include "gx-vk-lt-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../render/light/gx-rnd-lt-directional.hpp"
#include "../../render/light/gx-rnd-lt-point.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-lt-shadow-caster-directional.hpp"

gearoenix::vulkan::light::Manager::Manager()
    : Singleton<Manager>(this)
    , directionals_uniform_indexer(render::light::Directional::max_count)
    , points_uniform_indexer(render::light::Point::max_count)
    , directional_shadow_casters_uniform_indexer(ShadowCasterDirectional::max_count)
{
    core::ecs::ComponentType::add<ShadowCasterDirectional>();
}

gearoenix::vulkan::light::Manager::~Manager() = default;

void gearoenix::vulkan::light::Manager::update()
{
    directionals_uniform_indexer.reset();
    points_uniform_indexer.reset();
    directional_shadow_casters_uniform_indexer.reset();
    render::light::Manager::update();
}

void gearoenix::vulkan::light::Manager::upload_uniforms()
{
    directionals_uniform_indexer.update();
    points_uniform_indexer.update();
    directional_shadow_casters_uniform_indexer.update();
}

gearoenix::core::ecs::EntityPtr gearoenix::vulkan::light::Manager::build_directional(std::string&&, core::ecs::Entity* const)
{
    GX_UNIMPLEMENTED;
}

void gearoenix::vulkan::light::Manager::build_shadow_caster_directional(
    std::string&& name,
    core::ecs::Entity* const parent,
    const std::uint32_t shadow_map_resolution,
    const float camera_far, const float camera_near, const float camera_aspect,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    render::light::Manager::build_shadow_caster_directional(
        std::move(name), parent, shadow_map_resolution, camera_far, camera_near, camera_aspect,
        core::job::EndCaller<core::ecs::EntityPtr>([&](core::ecs::EntityPtr&& entity) -> void {
            auto* const e = entity.get();
            e->add_component(core::Object::construct<ShadowCasterDirectional>(e, e->get_object_name() + "-gl-directional-shadow-caster"));
            entity_callback.set_return(std::move(entity));
            e->get_component_shared_ptr<ShadowCasterDirectional>()->initialise(
                shadow_map_resolution, camera_far, camera_near, camera_aspect,
                core::job::EndCaller([e = std::move(entity_callback)] { (void)e; }));
        }));
}

#endif
