#include "gx-vk-scn-scene.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../gx-vk-marker.hpp"
#include "../shader/glsl/gx-vk-shd-common.glslh"
#include "gx-vk-scn-manager.hpp"
#include "../camera/gx-vk-cmr-camera.hpp"

#include <ranges>

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
    // TODO: update the data in the ptr
}

void gearoenix::vulkan::scene::Scene::render_shadows(const VkCommandBuffer vk_cmd)
{
    GX_VK_PUSH_DEBUG_GROUP(vk_cmd, 0.5f, 1.0f, 0.0f, "{}", shadow_render_pass_name);
    for (const auto& index : record.cameras.shadow_casters | std::views::values) {
        auto& cmr_rcd = record.cameras.cameras[index];
        core::cast_ptr<camera::Camera>(cmr_rcd.camera)->render_shadow(cmr_rcd, vk_cmd);
    }
}

void gearoenix::vulkan::scene::Scene::render_reflection_probes(const VkCommandBuffer vk_cmd) const
{
    GX_VK_PUSH_DEBUG_GROUP(vk_cmd, 0.5f, 1.0f, 0.5f, "{}", shadow_reflection_probe_render_pass_name);
    for (const auto ci : record.cameras.reflections | std::views::values) {
        auto& camera = record.cameras.cameras[ci];
        core::cast_ptr<camera::Camera>(camera.camera)->render_forward(*this, camera, vk_cmd);
    }
}

void gearoenix::vulkan::scene::Scene::render_forward(const VkCommandBuffer vk_cmd)
{
    render_reflection_probes(vk_cmd);
    GX_VK_PUSH_DEBUG_GROUP(vk_cmd, 0.5f, 0.0f, 0.5f, "{}", forward_render_pass_name);
    for (const auto camera_index : record.cameras.mains | std::views::values) {
        auto& camera = record.cameras.cameras[camera_index];
        auto& cam = *core::cast_ptr<camera::Camera>(camera.camera);
        cam.render_forward(*this, camera, vk_cmd);
        cam.render_bloom(*this, camera, vk_cmd);
        cam.render_colour_correction_anti_aliasing(*this, camera, vk_cmd);
    }
}

#endif
