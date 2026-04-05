#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/model/gx-rnd-mdl-manager.hpp"
#include "../../render/model/gx-rnd-mdl-model.hpp"
#include "../mesh/gx-vk-msh-draw-cache.hpp"

namespace gearoenix::render::record {
struct Camera;
struct CameraModel;
}

namespace gearoenix::vulkan {
struct DrawState;
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

    using draw_caches_t = boost::container::static_vector<mesh::DrawCache, render::model::max_meshes_count_per_model>;

    constexpr static auto object_type_index = gearoenix_gapi_model_type_index;
    constexpr static std::array all_parent_object_type_indices { render::model::Model::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::model::Model::object_type_index };

private:
    draw_caches_t draw_caches;
    std::uint64_t last_update_frame = static_cast<std::uint64_t>(-1);
    std::uint32_t shader_data_index = static_cast<std::uint32_t>(-1);

    Model(core::ecs::Entity* entity, render::model::meshes_set_t&& ms, std::string&& name, bool is_transformable, bool is_skinned);

public:
    [[nodiscard]] std::uint64_t get_last_update_frame() const { return last_update_frame; }
    [[nodiscard]] std::uint32_t get_shader_data_index() const { return shader_data_index; }

    ~Model() override;
    void render_shadow(DrawState& draw_state);
    void render_forward(DrawState& draw_state);
    void after_record(std::uint64_t frame_number, const render::record::CameraModel& rec_cam_mdl);
};
}
#endif