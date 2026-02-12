#include "gx-vk-scn-scene.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../gx-vk-marker.hpp"
#include "../shader/glsl/gx-vk-shd-common.glslh"
#include "gx-vk-scn-manager.hpp"
#include "../camera/gx-vk-cmr-camera.hpp"
#include "../texture/gx-vk-txt-manager.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"

#include <ranges>

namespace {
gearoenix::vulkan::texture::Texture2D *brdflut = nullptr;

void initialize_brdflut()
{
    if (brdflut) {
        return;
    }

    gearoenix::render::texture::Manager::get().get_brdflut(gearoenix::core::job::EndCallerShared<gearoenix::render::texture::Texture2D>([](auto&& t) {
        brdflut = gearoenix::core::cast_ptr<gearoenix::vulkan::texture::Texture2D>(t.get());
    }));
}
}

gearoenix::vulkan::scene::Scene::Scene(core::ecs::Entity* const e, std::string&& name, const double layer)
    : render::scene::Scene(e, core::ecs::ComponentType::create_index(this), layer, std::move(name))
{
    initialize_brdflut();
}

gearoenix::vulkan::scene::Scene::Scene(const core::object_id_t id, std::string&& name)
    : render::scene::Scene(core::ecs::ComponentType::create_index(this), id, std::move(name))
{
    initialize_brdflut();
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
}

void gearoenix::vulkan::scene::Scene::render_shadows(const VkCommandBuffer vk_cmd, VkPipeline& current_bound_pipeline)
{
    GX_VK_PUSH_DEBUG_GROUP(vk_cmd, 0.5f, 1.0f, 0.0f, "{}", shadow_render_pass_name);

    pipeline::PushConstants pc;
    pc.scene_index = shader_data_index;

    for (const auto& index : record.cameras.shadow_casters | std::views::values) {
        auto& cmr_rcd = record.cameras.cameras[index];
        core::cast_ptr<camera::Camera>(cmr_rcd.camera)->render_shadow(cmr_rcd, vk_cmd, pc, current_bound_pipeline);
    }
}

void gearoenix::vulkan::scene::Scene::render_reflection_probes(const VkCommandBuffer vk_cmd, VkPipeline& current_bound_pipeline) const
{
    GX_VK_PUSH_DEBUG_GROUP(vk_cmd, 0.5f, 1.0f, 0.5f, "{}", shadow_reflection_probe_render_pass_name);

    pipeline::PushConstants pc;
    pc.scene_index = shader_data_index;

    for (const auto ci : record.cameras.reflections | std::views::values) {
        auto& camera = record.cameras.cameras[ci];
        core::cast_ptr<camera::Camera>(camera.camera)->render_forward(camera, vk_cmd, pc, current_bound_pipeline);
    }
}

void gearoenix::vulkan::scene::Scene::render_forward(const VkCommandBuffer vk_cmd, VkPipeline& current_bound_pipeline)
{
    // render_reflection_probes(vk_cmd);
    GX_VK_PUSH_DEBUG_GROUP(vk_cmd, 0.5f, 0.0f, 0.5f, "{}", forward_render_pass_name);

    pipeline::PushConstants pc;
    pc.scene_index = shader_data_index;

    for (const auto camera_index : record.cameras.mains | std::views::values) {
        auto& rc = record.cameras.cameras[camera_index];
        auto& cam = *core::cast_ptr<camera::Camera>(rc.camera);
        cam.render_forward(rc, vk_cmd, pc, current_bound_pipeline);
        // cam.render_bloom(*this, camera, vk_cmd); // TODO
        // cam.render_colour_correction_anti_aliasing(*this, camera, vk_cmd); // TODO
    }
}

void gearoenix::vulkan::scene::Scene::after_record(const std::uint64_t frame_number)
{
    auto sd = descriptor::UniformIndexer<GxShaderDataScene>::get().get_next();
    shader_data_index = sd.get_index();
    auto& ref = *sd.get_ptr();
    ref.ambient_light = math::Vec4(0.001f); // TODO: Temporary later, render module must provide it.
    GX_ASSERT_D(brdflut);
    ref.brdflut_texture_index = brdflut->get_view_index();
    ref.brdflut_sampler_index = brdflut->get_sampler_index();

    const auto& cameras = record.cameras;
    for (auto cam_i = 0; cam_i < cameras.last_camera_index; ++cam_i) {
        auto& rc = cameras.cameras[cam_i];
        core::cast_ptr<camera::Camera>(rc.camera)->after_record(frame_number, rc);
    }
}

#endif
