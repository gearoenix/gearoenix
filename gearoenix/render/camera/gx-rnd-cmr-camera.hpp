#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../../math/gx-math-ray.hpp"
#include "gx-rnd-cmr-bloom-data.hpp"
#include "gx-rnd-cmr-colour-tuning.hpp"
#include "gx-rnd-cmr-exposure.hpp"
#include "gx-rnd-cmr-projection.hpp"
#include "gx-rnd-cmr-target.hpp"
#include <array>
#include <optional>

#ifdef projection
#undef projection
#endif
#ifdef far
#undef far
#endif
#ifdef near
#undef near
#endif

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::camera {
struct Camera : core::ecs::Component {
    friend struct Builder;
    friend struct Manager;

    constexpr static core::ecs::component_index_t TYPE_INDEX = 18;
    constexpr static std::uint32_t MAX_COUNT = 16;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    enum struct Usage : std::uint8_t {
        ReflectionProbe = 5,
        Shadow = 11,
        Main = 23,
    };

    engine::Engine& e;
    GX_GET_CREF_PRT(math::Mat4x4<float>, view);
    GX_GET_CREF_PRT(math::Mat4x4<float>, projection);
    GX_GET_CREF_PRT(math::Mat4x4<float>, view_projection);
    GX_GET_CREF_PRT(math::Vec4<float>, starting_clip_ending_clip);
    GX_GET_CREF_PRT(Target, target);
    GX_GET_CREF_PRT(std::optional<float>, customised_target_aspect_ratio);
    GX_GETSET_VAL_PRT(core::ecs::entity_id_t, parent_entity_id, core::ecs::invalid_entity_id); // It can be light or reflection probe or any other owner entity
    GX_GETSET_VAL_PRT(core::ecs::entity_id_t, scene_id, core::ecs::invalid_entity_id);
    GX_GETSET_VAL_PRT(std::uint64_t, flag, 1);
    GX_GET_VAL_PRT(float, far, 100.0f);
    GX_GET_VAL_PRT(float, near, 1.0f);
    GX_GET_REF_PRT(ColourTuning, colour_tuning);
    GX_GET_VAL_PRT(ProjectionData, projection_data, PerspectiveProjectionData {});
    GX_GETSET_VAL_PRT(double, layer, 0.0);
    GX_GETSET_VAL_PRT(Usage, usage, Usage::Main);
    GX_GET_VAL_PRT(bool, debug_enabled, false);
    GX_GET_REFC_PRT(math::Vec4<float>, debug_colour);
    GX_GET_CREF_PRT(std::shared_ptr<mesh::Mesh>, debug_mesh);
    GX_GET_CREF_PRT(std::optional<BloomData>, bloom_data);
    GX_GET_CREF_PRT(Exposure, exposure);
    GX_GET_VAL_PRT(std::uint32_t, resolution_cfg_observer, 0);
    GX_GET_CREF_PRV(std::weak_ptr<Camera>, camera_self);
    GX_GET_CREF_PRV(std::shared_ptr<physics::Transformation>, transform);

protected:
    Camera(
        engine::Engine& e,
        core::ecs::component_index_t final_type,
        const std::string& name,
        Target&& target,
        std::shared_ptr<physics::Transformation>&& transform,
        core::ecs::entity_id_t entity_id);

    void set_camera_self(const std::shared_ptr<Camera>& camera_self);

public:
    ~Camera() override;
    void generate_frustum_points(
        const math::Vec3<double>& location,
        const math::Vec3<double>& x,
        const math::Vec3<double>& y,
        const math::Vec3<double>& z,
        std::array<math::Vec3<double>, 8>& points) const;
    void set_view(const math::Mat4x4<float>& view);
    void set_customised_target_aspect_ratio(float target_aspect_ratio);
    void disable_customised_target_aspect_ratio();
    /// This returns customised_target_aspect_ratio value if it is set or the actual target aspect ratio
    [[nodiscard]] float get_target_aspect_ratio() const;
    void set_projection_data(ProjectionData);
    [[nodiscard]] bool is_perspective() const;
    [[nodiscard]] bool is_orthographic() const;
    void update_projection();
    void set_near(float);
    void set_far(float);
    void show_debug_gui() override;
    void enable_debug_mesh(core::job::EndCaller<>&& end);
    void disable_debug_mesh();
    [[nodiscard]] math::Ray3<double> generate_ray(const math::Vec2<double>& normalised_point) const;
    virtual void set_customised_target(std::shared_ptr<texture::Target>&&);
    virtual void create_debug_mesh(core::job::EndCaller<>&& end);
    virtual void disable_bloom();
    virtual void enable_bloom();
    virtual void update_bloom();
    virtual void update_target(core::job::EndCaller<>&& end);
};
}