#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/model/gx-rnd-mdl-manager.hpp"
#include "../render/model/gx-rnd-mdl-model.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::render::record {
struct Camera;
struct CameraModel;
}

namespace gearoenix::gl {
struct Engine;
struct Mesh;
struct Scene;

struct Model final : render::model::Model {
    GEAROENIX_OBJECT_STRUCT_DEF;

    typedef core::static_flat_set<std::shared_ptr<Mesh>, render::model::max_meshes_count_per_model> gl_meshes_set_t;

    constexpr static auto object_type_index = gearoenix_gl_model_type_index;
    constexpr static std::array all_parent_object_type_indices { render::model::Model::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::model::Model::object_type_index };

    GX_GET_CREF_PRV(gl_meshes_set_t, gl_meshes);

    Model(core::ecs::Entity* entity, render::model::meshes_set_t&& ms, std::string&& name, bool is_transformable);

public:
    ~Model() override;
    void render_shadow(
        const render::record::Camera& camera,
        const render::record::CameraModel& camera_model,
        uint& current_shader);
    void render_forward(
        const Scene& scene,
        const render::record::Camera& camera,
        const render::record::CameraModel& camera_model,
        uint& current_shader);
};

struct ModelManager final : render::model::Manager {
private:
    [[nodiscard]] core::ecs::EntityPtr build(
        std::string&& name,
        core::ecs::Entity* parent,
        render::model::meshes_set_t&& meshes,
        bool is_transformable) override;

public:
    ModelManager();
    ~ModelManager() override;
};
}

#endif
