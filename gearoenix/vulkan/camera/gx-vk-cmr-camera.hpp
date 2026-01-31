#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/camera/gx-rnd-cmr-camera.hpp"
#include "../../render/record/gx-rnd-rcd-camera.hpp"
#include "gx-vk-cmr-target.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::render::record {
struct Model;
}

namespace gearoenix::vulkan::pipeline {
struct BloomPrefilter;
struct BloomHorizontal;
struct BloomVertical;
struct BloomUpsampler;
struct Multiply;
struct Pipeline;
struct SkyboxCube;
struct SkyboxEquirectangular;
struct ColourTuningAntiAliasingCombination;
}

namespace gearoenix::vulkan::scene {
struct Scene;
}

namespace gearoenix::vulkan::camera {
struct Camera final : render::camera::Camera {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto object_type_index = gearoenix_gapi_camera_type_index;
    constexpr static std::array all_parent_object_type_indices { render::camera::Camera::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::camera::Camera::object_type_index };

    GX_GET_CREF_PRV(Target, gapi_target);

    GX_GET_CREF_PRV(std::shared_ptr<pipeline::BloomPrefilter>, bloom_prefilter);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::BloomHorizontal>, bloom_horizontal);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::BloomVertical>, bloom_vertical);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::BloomUpsampler>, bloom_upsampler);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::Multiply>, multiply);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::SkyboxCube>, skybox_cube);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::SkyboxEquirectangular>, skybox_equirectangular);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::ColourTuningAntiAliasingCombination>, colour_tuning_anti_aliasing_combination);

    struct RecVkModel final {
        render::record::Model* model = nullptr;
        std::uint32_t first_mvp_index = static_cast<std::uint32_t>(-1);
        std::uint32_t mvps_count = static_cast<std::uint32_t>(-1);
    };

    std::vector<std::uint32_t> cameras_joint_model_indices; // MVPs
    std::vector<RecVkModel> vk_rec_opaque_models;
    std::vector<RecVkModel> vk_rec_translucent_models;
    std::uint32_t camera_uniform_index = static_cast<std::uint32_t>(-1);


    void set_customised_target(std::shared_ptr<render::texture::Target>&&) override;
    void update_target(core::job::EndCaller<>&& end) override;

    Camera(core::ecs::Entity* entity, const std::string& name, render::camera::Target&& target, std::shared_ptr<physics::Transformation>&& transform);

public:
    static void construct(core::ecs::Entity* entity, const std::string& name, core::job::EndCallerShared<Camera>&& c, std::shared_ptr<physics::Transformation>&& transform);
    ~Camera() override;
    void render_shadow(const render::record::Camera&, VkCommandBuffer cmd) const;
    void render_forward(const scene::Scene& scene, const render::record::Camera&, VkCommandBuffer cmd) const;
    void render_forward_skyboxes(const scene::Scene& scene, const render::record::Camera&, VkCommandBuffer cmd) const;
    void render_bloom(const scene::Scene& scene, const render::record::Camera&, VkCommandBuffer cmd) const;
    void render_colour_correction_anti_aliasing(const scene::Scene& scene, const render::record::Camera&, VkCommandBuffer cmd) const;
    void after_record(const render::record::Camera& rc);
};
}
#endif