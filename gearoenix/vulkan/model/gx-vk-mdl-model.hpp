#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/model/gx-rnd-mdl-manager.hpp"
#include "../../render/model/gx-rnd-mdl-model.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::render::record {
struct Camera;
struct CameraModel;
}

namespace gearoenix::vulkan::mesh {
struct Mesh;
}

namespace gearoenix::vulkan::pipeline {
struct PushConstants;
}

namespace gearoenix::vulkan::scene {
struct Scene;
}


namespace gearoenix::vulkan::model {
struct Model final : render::model::Model {
    GEAROENIX_OBJECT_STRUCT_DEF;

    typedef core::static_flat_set<std::shared_ptr<mesh::Mesh>, render::model::max_meshes_count_per_model> gapi_meshes_set_t;

    constexpr static auto object_type_index = gearoenix_gapi_model_type_index;
    constexpr static std::array all_parent_object_type_indices { render::model::Model::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::model::Model::object_type_index };

    GX_GET_CREF_PRV(gapi_meshes_set_t, gapi_meshes);
    GX_GET_VAL_PRV(std::uint32_t, shader_data_index, static_cast<std::uint32_t>(-1));

    Model(core::ecs::Entity* entity, render::model::meshes_set_t&& ms, std::string&& name, bool is_transformable);

public:
    ~Model() override;
    void render_shadow(
        const render::record::Camera& camera,
        const render::record::CameraModel& camera_model,
        VkCommandBuffer cmd,
        pipeline::PushConstants& pc,
        VkPipeline& current_bound_pipeline);
    void render_forward(
        const scene::Scene& scene,
        const render::record::Camera& camera,
        const render::record::CameraModel& camera_model,
        VkCommandBuffer cmd,
        pipeline::PushConstants& pc,
        VkPipeline& current_bound_pipeline);
    void after_record(const render::record::CameraModel& rec_cam_mdl);
};
}
#endif