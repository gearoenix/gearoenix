#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../../math/gx-math-ray.hpp"
#include "../gx-rnd-resolution.hpp"
#include "gx-rnd-cmr-colour-tuning.hpp"
#include "gx-rnd-cmr-projection.hpp"
#include <array>

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

namespace gearoenix::render::texture {
struct Target;
}

namespace gearoenix::render::camera {
struct Camera final : public core::ecs::Component {
    friend struct Builder;
    friend struct Manager;

    enum struct Usage : std::uint8_t {
        ReflectionProbe = 5,
        Shadow = 11,
        Main = 23,
    };

    engine::Engine& e;
    GX_GET_CREF_PRV(math::Mat4x4<float>, view);
    GX_GET_CREF_PRV(math::Mat4x4<float>, projection);
    GX_GET_CREF_PRV(math::Mat4x4<float>, view_projection);
    GX_GET_CREF_PRV(math::Vec4<float>, starting_clip_ending_clip);
    GX_GET_VAL_PRV(bool, has_customised_target, false);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, target);
    GX_GET_VAL_PRV(bool, has_customised_target_aspect_ratio, false);
    GX_GET_VAL_PRV(float, target_aspect_ratio, 1.7f);
    GX_GETSET_VAL_PRV(core::ecs::Entity::id_t, reference_id, 0); // It can be light or reflection probe or any other owner entity TODO: remove this, all of the camera component users must maintain one entity
    GX_GETSET_VAL_PRV(core::ecs::Entity::id_t, scene_id, 0);
    GX_GETSET_VAL_PRV(std::uint64_t, flag, 1);
    GX_GET_VAL_PRV(float, far, 100.0f);
    GX_GET_VAL_PRV(float, near, 1.0f);
    GX_GET_VAL_PRV(float, scale_fov_y, 0.8f);
    GX_GET_REF_PRV(ColourTuning, colour_tuning);
    GX_GET_VAL_PRV(Projection, projection_type, Projection::Perspective);
    GX_GETSET_VAL_PRV(double, layer, 0.0);
    GX_GETSET_VAL_PRV(Usage, usage, Usage::Main);
    GX_GET_VAL_PRV(bool, debug_enabled, false);
    GX_GET_REFC_PRV(math::Vec4<float>, debug_colour);
    GX_GET_CREF_PRV(std::shared_ptr<mesh::Mesh>, debug_mesh);
    GX_GETSET_VAL_PRV(bool, has_bloom, true);
    GX_GET_VAL_PRV(std::size_t, resolution_cfg_listener, 0);

public:
    Camera(
        engine::Engine& e,
        const std::string& name,
        std::size_t resolution_cfg_listener,
        const core::sync::EndCaller& end_caller,
        std::shared_ptr<texture::Target>&& customised_target = nullptr,
        Projection projection_type = Projection::Perspective,
        float near = 1.0f,
        float far = 100.0f) noexcept;
    Camera(Camera&&) noexcept;
    ~Camera() noexcept final;
    void generate_frustum_points(
        const math::Vec3<double>& location,
        const math::Vec3<double>& x,
        const math::Vec3<double>& y,
        const math::Vec3<double>& z,
        std::array<math::Vec3<double>, 8>& points) const noexcept;
    void set_view(const math::Mat4x4<float>& view) noexcept;
    void set_customised_target_aspect_ratio(float target_aspect_ratio) noexcept;
    void set_projection_type(Projection) noexcept;
    void update_projection() noexcept;
    void set_near(float) noexcept;
    void set_far(float) noexcept;
    void set_fov_y(float) noexcept;
    void set_scale(float) noexcept;
    void show_gui() noexcept;
    void enable_debug_mesh() noexcept;
    void disable_debug_mesh() noexcept;
    void set_has_customised_target_aspect_ratio(bool b) noexcept;
    [[nodiscard]] math::Ray3<double> generate_ray(
        const physics::Transformation& transform, const math::Vec2<double>& normalised_point) const noexcept;

private:
    void update_target_aspect_ratio() noexcept;
    void set_target_aspect_ratio(float target_aspect_ratio) noexcept;
    /// It must be changed by Builder or Manager, because the other components must know about the change
    void set_customised_target(std::shared_ptr<texture::Target>&&) noexcept;
    void update_target() noexcept;
    void create_debug_mesh() noexcept;
};
}
#endif
