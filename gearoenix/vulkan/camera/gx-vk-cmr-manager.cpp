#include "gx-vk-cmr-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-cmr-camera.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/gx-phs-transformation.hpp"

gearoenix::vulkan::camera::Manager::Manager() : Singleton<Manager>(this)
{
}

gearoenix::vulkan::camera::Manager::~Manager() = default;

void gearoenix::vulkan::camera::Manager::build(std::string&& name, core::ecs::Entity* parent, core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    build_impl(std::move(name), parent, entity_callback);
    auto* const entity = entity_callback.get_return().get();
    auto transform = entity->get_component_shared_ptr<physics::Transformation>();
    Camera::construct(
        entity,
        entity->get_object_name() + "-vk-camera",
        core::job::EndCallerShared<Camera>([end = std::move(entity_callback)](std::shared_ptr<Camera>&& c) {
            end.get_return()->add_component(std::move(c));
        }),
        std::move(transform));
}

void gearoenix::vulkan::camera::Manager::window_resized()
{
    render::camera::Manager::window_resized();
}
#endif
