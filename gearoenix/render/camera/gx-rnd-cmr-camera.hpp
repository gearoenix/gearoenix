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

    constexpr static auto object_type_index = gearoenix_render_camera_type_index;
    constexpr static auto max_count = 16;

    enum struct Usage : std::uint8_t {
        ReflectionProbe = 5,
        Shadow = 11,
        Main = 23,
    };

    GX_GET_CREF_PRT(math::Mat4x4<float>, view);
    GX_GET_CREF_PRT(math::Mat4x4<float>, projection);
    GX_GET_CREF_PRT(math::Mat4x4<float>, view_projection);
    GX_GET_CREF_PRT(math::Vec4<float>, starting_clip_ending_clip);
    GX_GET_CREF_PRT(Target, target);
    GX_GET_CREF_PRT(std::optional<float>, customised_target_aspect_ratio);
    GX_GETSET_VAL_PRT(std::uint64_t, flag, 1);
    GX_GET_VAL_PRT(float, far, 100.0f);
    GX_GET_VAL_PRT(float, near, 1.0f);
    GX_GET_REF_PRT(ColourTuning, colour_tuning);
    GX_GET_CREF_PRT(ProjectionData, projection_data);
    GX_GETSET_VAL_PRT(double, layer, 0.0);
    GX_GETSET_VAL_PRT(Usage, usage, Usage::Main);
    GX_GET_REFC_PRT(math::Vec4<float>, debug_colour);
    GX_GET_CREF_PRT(std::shared_ptr<mesh::Mesh>, debug_mesh); // TODO: remove this, Frustum collider will have gizmo
    GX_GET_CREF_PRT(std::optional<BloomData>, bloom_data);
    GX_GET_CREF_PRT(Exposure, exposure);
    GX_GET_VAL_PRT(std::uint32_t, resolution_cfg_observer, 0);
    GX_GET_CREF_PRT(std::shared_ptr<physics::Transformation>, transform);

    Camera(core::object_type_index_t final_type, const std::string& name, Target&& target, std::shared_ptr<physics::Transformation>&& transform);

    /// It should be called exactly after the shared_ptr got generated, and it has a valid weak_ptr of self.
    void initialise();
    void write(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) const;
    void read(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&);
    void show_debug_gui() override;

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