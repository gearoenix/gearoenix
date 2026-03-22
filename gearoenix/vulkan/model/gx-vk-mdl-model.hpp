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
struct FormatPipelines;
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

private:
    gapi_meshes_set_t gapi_meshes;
    std::uint64_t last_update_frame = static_cast<std::uint64_t>(-1);
    std::uint32_t shader_data_index = static_cast<std::uint32_t>(-1);

    Model(core::ecs::Entity* entity, render::model::meshes_set_t&& ms, std::string&& name, bool is_transformable);

public:
    [[nodiscard]] const gapi_meshes_set_t& get_gapi_meshes() const { return gapi_meshes; }
    [[nodiscard]] std::uint64_t get_last_update_frame() const { return last_update_frame; }
    [[nodiscard]] std::uint32_t get_shader_data_index() const { return shader_data_index; }

    ~Model() override;
    void render_shadow(const render::record::CameraModel& camera_model, vk::CommandBuffer cmd, pipeline::PushConstants& pc, vk::Pipeline& current_bound_pipeline);
    void render_forward(const render::record::CameraModel& camera_model, const pipeline::FormatPipelines& fp, vk::CommandBuffer cmd, pipeline::PushConstants& pc, vk::Pipeline& current_bound_pipeline);
    void after_record(std::uint64_t frame_number, const render::record::CameraModel& rec_cam_mdl);
};
}
#endif