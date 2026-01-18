#include "gx-vk-scn-scene.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "gx-vk-scn-manager.hpp"
#include "../shader/glsl/gx-vk-shd-common.glsl"

gearoenix::vulkan::scene::Scene::Scene(core::ecs::Entity* const e, std::string&& name, const double layer)
    : render::scene::Scene(e, core::ecs::ComponentType::create_index(this), layer, std::move(name))
{
}

gearoenix::vulkan::scene::Scene::Scene(const core::object_id_t id, std::string&& name)
    : render::scene::Scene(core::ecs::ComponentType::create_index(this), id, std::move(name))
{
}

void gearoenix::vulkan::scene::Scene::read(
    std::shared_ptr<Scene>&& self, std::shared_ptr<platform::stream::Stream>&& stream,
    std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end)
{
    render::scene::Scene::read(std::shared_ptr<render::scene::Scene>(std::move(self)), std::move(stream), std::move(object_streamer), std::move(end));
}

gearoenix::vulkan::scene::Scene::~Scene() = default;

void gearoenix::vulkan::scene::Scene::update()
{
    render::scene::Scene::update();
    const auto [ptr, index] = Manager::get_shader_data();
    shader_data_index = index;
    ptr->dummy = 0;
}

void gearoenix::vulkan::scene::Scene::render_shadows(uint& current_shader)
{
    GX_UNIMPLEMENTED;
}

void gearoenix::vulkan::scene::Scene::render_reflection_probes(uint& current_shader) const
{
    GX_UNIMPLEMENTED;
}

void gearoenix::vulkan::scene::Scene::render_forward(uint& current_shader)
{
    GX_UNIMPLEMENTED;
}

#endif
