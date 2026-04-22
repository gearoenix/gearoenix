#include "gx-vk-scn-scene.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../camera/gx-vk-cmr-camera.hpp"
#include "../gx-vk-draw-state.hpp"
#include "../gx-vk-marker.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"
#include "../skybox/gx-vk-sky-skybox.hpp"
#include "../texture/gx-vk-txt-2d.hpp"

#include <ranges>

gearoenix::vulkan::scene::Scene::Scene(core::ecs::Entity* const e, std::string&& name, const core::fp_t layer, std::shared_ptr<render::texture::Texture2D>&& brdflut)
    : render::scene::Scene(e, core::ecs::ComponentType::create_index(this), layer, std::move(name), std::move(brdflut))
{
}

void gearoenix::vulkan::scene::Scene::read(
    std::shared_ptr<Scene>&& self,
    std::shared_ptr<platform::stream::Stream>&& stream,
    std::shared_ptr<core::ObjectStreamer>&& object_streamer,
    core::job::EndCaller<>&& end)
{
    render::scene::Scene::read(
        std::shared_ptr<render::scene::Scene>(std::move(self)),
        std::move(stream),
        std::move(object_streamer),
        std::move(end));
}

gearoenix::vulkan::scene::Scene::~Scene() = default;

void gearoenix::vulkan::scene::Scene::render_shadows(DrawState& draw_state)
{
    GX_VK_PUSH_DEBUG_GROUP(draw_state.command_buffer, 0.5f, 1.0f, 0.0f, "Shadow pass of Scene {}", object_name);

    draw_state.push_constants.scene_index = uniform.shader_data_index;

    for (const auto& index : record.cameras.shadow_casters | std::views::values) {
        auto& cmr_rcd = record.cameras.cameras[index];
        core::cast_ptr<camera::Camera>(cmr_rcd.camera)->render_shadow(cmr_rcd, draw_state);
    }
}

void gearoenix::vulkan::scene::Scene::render_reflection_probes(DrawState& draw_state) const
{
    GX_VK_PUSH_DEBUG_GROUP(draw_state.command_buffer, 0.5f, 1.0f, 0.5f, "Reflection probe pass of Scene: {}", object_name);

    for (const auto ci : record.cameras.reflections | std::views::values) {
        auto& camera = record.cameras.cameras[ci];
        core::cast_ptr<camera::Camera>(camera.camera)->render_forward(camera, record.skyboxes, draw_state);
    }
}

void gearoenix::vulkan::scene::Scene::render_forward(DrawState& draw_state)
{
    draw_state.push_constants.scene_index = uniform.shader_data_index;

    render_reflection_probes(draw_state);

    GX_VK_PUSH_DEBUG_GROUP(draw_state.command_buffer, 0.5f, 0.0f, 0.5f, "Forward pass scene: {}", object_name);

    for (const auto camera_index : record.cameras.mains | std::views::values) {
        auto& rc = record.cameras.cameras[camera_index];
        auto& cam = *core::cast_ptr<camera::Camera>(rc.camera);
        cam.render_forward(rc, record.skyboxes, draw_state);
        cam.render_bloom(*this, draw_state.command_buffer);
        cam.render_colour_correction_anti_aliasing(*this, draw_state.command_buffer);
    }
}

#endif
