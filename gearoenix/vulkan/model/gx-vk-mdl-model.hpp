#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
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

    Model(
        core::ecs::Entity* entity,
        bool is_transformable,
        std::shared_ptr<physics::Transformation>&& transformation,
        render::model::meshes_set_t&&,
        std::string&& name,
        std::shared_ptr<physics::animation::Armature>&&);

public:
    ~Model() override;
    void render_shadow(DrawState& draw_state);
    void render_forward(DrawState& draw_state);
};
}
#endif