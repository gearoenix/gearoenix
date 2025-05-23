#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/gx-cr-static-flat-set.hpp"

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::model {
constexpr auto max_meshes_count_per_model = 16;
constexpr auto max_filtered_cameras_count = 8;

typedef core::static_flat_set<std::shared_ptr<mesh::Mesh>, max_meshes_count_per_model> meshes_set_t;
typedef core::static_flat_set<camera::Camera*, max_filtered_cameras_count> filtered_cameras_set_t;

struct Model : core::ecs::Component {
    constexpr static auto object_type_index = gearoenix_render_model_type_index;
    constexpr static auto max_count = 8192;

    GX_GET_VAL_PRT(bool, is_transformable, false);
    GX_GET_CREF_PRT(meshes_set_t, meshes);

    /// \note A model can be static while it has a transform component.
    Model(core::object_type_index_t final_component_type, bool is_transformable, meshes_set_t&& meshes, std::string&& name);

public:
    /// By having a value (even an empty one) we show that we want to have camera filtering enabled.
    /// If it has value but the set is empty, it will be filtered by all the cameras.
    std::optional<filtered_cameras_set_t> cameras;
    std::uint64_t cameras_flags = static_cast<std::uint64_t>(-1);

    ~Model() override;
    void show_debug_gui() override;
    [[nodiscard]] bool has_transparent_material() const;
    [[nodiscard]] bool needs_mvp() const;
};
}