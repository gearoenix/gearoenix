#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/gx-cr-static-flat-set.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"

struct GxShaderDataCameraJointModel;
struct GxShaderDataModel;

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::animation {
struct Armature;
}

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

    GX_GET_REF_PRT(buffer::Uniform, uniform);
    GX_GET_CREF_PRT(std::optional<buffer::Uniform>, bones_uniform);
    GX_GET_CREF_PRT(std::shared_ptr<physics::Transformation>, transform);
    GX_GET_VAL_PRT(bool, is_transformable, false);
    GX_GET_CREF_PRT(meshes_set_t, meshes);
    GX_GET_CREF_PRT(std::shared_ptr<physics::animation::Armature>, armature);

    /// \note A model can be static while it has a transform component.
    Model(
        core::ecs::Entity* entity,
        core::object_type_index_t final_component_type,
        bool is_transformable,
        std::shared_ptr<physics::Transformation>&& transformation,
        meshes_set_t&& meshes,
        std::string&& name,
        std::shared_ptr<physics::animation::Armature>&& armature);

public:
    /// By having a value (even an empty one) we show that we want to have camera filtering enabled.
    /// If it has value but the set is empty, it will be filtered by all the cameras.
    std::optional<filtered_cameras_set_t> cameras;
    std::uint64_t cameras_flags = static_cast<std::uint64_t>(-1);

    ~Model() override;
    void show_debug_gui() override;
    void update_uniform();

    /// After armature change, it is required to update bones uniforms.
    void create_bones_uniforms();
    void update_bones_uniform(const math::Mat4x4<float>& view_projection, GxShaderDataCameraJointModel* mvp_uniform_arr_ptr);

    [[nodiscard]] bool has_transparent_material() const;
    [[nodiscard]] bool needs_mvp() const;
    [[nodiscard]] std::uint32_t get_bones_count() const;
    [[nodiscard]] const GxShaderDataModel& get_uniform_inner_pointer() const;
    [[nodiscard]] GxShaderDataModel& get_uniform_inner_pointer();
};
}