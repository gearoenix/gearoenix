#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/scene/gx-rnd-scn-scene.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::pipeline {
struct PushConstants;
}

namespace gearoenix::vulkan::texture {
struct Texture2D;
}

namespace gearoenix::vulkan::scene {
struct Scene final : render::scene::Scene {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto object_type_index = gearoenix_gapi_scene_type_index;
    constexpr static std::array all_parent_object_type_indices { render::scene::Scene::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::scene::Scene::object_type_index };

    std::uint32_t shader_data_index = static_cast<std::uint32_t>(-1);
    std::shared_ptr<texture::Texture2D> brdflut;

    Scene(core::ecs::Entity* e, std::string&& name, core::fp_t layer);
    Scene(core::object_id_t id, std::string&& name);
    static void read(std::shared_ptr<Scene>&& self, std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end);
    void initialise_brdflut();

public:
    [[nodiscard]] std::uint32_t get_shader_data_index() const { return shader_data_index; }

    ~Scene() override;
    void update() override;
    void render_shadows(vk::CommandBuffer vk_cmd, vk::Pipeline& current_bound_pipeline);
    void render_reflection_probes(vk::CommandBuffer vk_cmd, pipeline::PushConstants& pc, vk::Pipeline& current_bound_pipeline) const;
    void render_forward(vk::CommandBuffer vk_cmd, vk::Pipeline& current_bound_pipeline);
    void after_record(std::uint64_t frame_number);
};
}
#endif