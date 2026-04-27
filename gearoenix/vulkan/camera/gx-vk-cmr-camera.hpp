#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/camera/gx-rnd-cmr-camera.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-cmr-target.hpp"

namespace gearoenix::render::record {
struct Camera;
struct Model;
struct Skyboxes;
}

namespace gearoenix::vulkan {
struct DrawState;
}

namespace gearoenix::vulkan::pipeline {
struct FormatPipelines;
struct Pipeline;
struct PushConstants;
struct SkyboxCube;
struct SkyboxEquirectangular;
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

    [[nodiscard]] const Target& get_gapi_target() const { return gapi_target; }
    [[nodiscard]] const std::shared_ptr<pipeline::SkyboxCube>& get_skybox_cube() const { return skybox_cube; }
    [[nodiscard]] const std::shared_ptr<pipeline::SkyboxEquirectangular>& get_skybox_equirectangular() const { return skybox_equirectangular; }

private:
    Target gapi_target;
    std::shared_ptr<pipeline::SkyboxCube> skybox_cube;
    std::shared_ptr<pipeline::SkyboxEquirectangular> skybox_equirectangular;

    vk::raii::DescriptorPool bloom_descriptor_pool { nullptr };
    std::array<vk::DescriptorSet, GX_RENDER_DEFAULT_CAMERA_TARGET_MIPS_COUNT> bloom_ds_tex0_to_tex1 { };
    std::array<vk::DescriptorSet, GX_RENDER_DEFAULT_CAMERA_TARGET_MIPS_COUNT> bloom_ds_tex1_to_tex0 { };

    void initialise_bloom_descriptors();
    void destroy_bloom_descriptors();

    vk::Format rendering_colour_format = vk::Format::eUndefined;

    Camera(core::ecs::Entity* entity, const std::string& name, render::camera::Target&& target, std::shared_ptr<physics::Transformation>&& transform);

public:
    static void construct(core::ecs::Entity* entity, const std::string& name, core::job::EndCallerShared<Camera>&& c, std::shared_ptr<physics::Transformation>&& transform);
    ~Camera() override;
    void render_shadow(const render::record::Camera&, DrawState& draw_state) const;
    void render_forward(const render::record::Camera&, const render::record::Skyboxes&, DrawState& draw_state) const;
    void render_forward_skyboxes(const render::record::Skyboxes&, DrawState& draw_state) const;
    void render_bloom(const scene::Scene& scene, vk::CommandBuffer cmd) const;
    void render_colour_tuning(const scene::Scene& scene, vk::CommandBuffer cmd) const;
    void set_customised_target(std::shared_ptr<render::texture::Target>&&) override;
    void update_target(core::job::EndCaller<>&& end) override;
    static void record_viewport(const render::record::Camera&, vk::CommandBuffer);
};
}
#endif
