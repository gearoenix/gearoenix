#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/scene/gx-rnd-scn-scene.hpp"
#include "../gx-vk-loader.hpp"

struct ShaderDataScene;

namespace gearoenix::vulkan::scene {
struct Scene final : render::scene::Scene {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto object_type_index = gearoenix_gapi_scene_type_index;
    constexpr static std::array all_parent_object_type_indices { render::scene::Scene::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::scene::Scene::object_type_index };

    GX_GET_VAL_PRV(std::uint32_t, shader_data_index, static_cast<std::uint32_t>(-1));

    Scene(core::ecs::Entity* e, std::string&& name, double layer);
    Scene(core::object_id_t id, std::string&& name);
    static void read(
        std::shared_ptr<Scene>&& self, std::shared_ptr<platform::stream::Stream>&& stream,
        std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end);

public:
    ~Scene() override;
    void update() override;
    void render_shadows(VkCommandBuffer vk_cmd);
    void render_reflection_probes(VkCommandBuffer vk_cmd) const;
    void render_forward(VkCommandBuffer vk_cmd);
    void after_record();
};
}
#endif