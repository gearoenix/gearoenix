#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
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

namespace gearoenix::render::texture {
struct Target;
}

namespace gearoenix::render::camera {
struct Camera final : public core::ecs::Component {
    friend struct Builder;
    friend struct Manager;

    enum struct Usage {
        ReflectionProbe,
        Shadow,
        Main,
    };

    GX_GET_CREF_PRV(math::Mat4x4<float>, view)
    GX_GET_CREF_PRV(math::Mat4x4<float>, projection)
    GX_GET_CREF_PRV(math::Mat4x4<float>, view_projection)
    GX_GET_CREF_PRV(math::Vec4<float>, starting_clip_ending_clip)
    GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, target)
    GX_GETSET_VAL_PRV(core::ecs::Entity::id_t, scene_id, 0)
    GX_GETSET_VAL_PRV(std::uint64_t, flag, 1)
    GX_GET_VAL_PRV(float, target_aspect_ratio, 1.7f)
    GX_GET_VAL_PRV(float, far, 100.0f)
    GX_GET_VAL_PRV(float, near, 1.0f)
    GX_GET_VAL_PRV(float, scale, 1.0f)
    GX_GETSET_VAL_PRV(float, hdr_tune_mapping, 1.0f)
    GX_GETSET_VAL_PRV(float, gamma_correction, 2.2f)
    GX_GET_VAL_PRV(Projection, projection_type, Projection::Perspective)
    GX_GETSET_VAL_PRV(double, layer, 0.0)
    GX_GETSET_VAL_PRV(Usage, usage, Usage::Main)

    Camera(
        float target_aspect_ratio,
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
    void set_target_aspect_ratio(float target_aspect_ratio) noexcept;
    void set_projection_type(Projection) noexcept;
    void update_projection() noexcept;
    void set_near(float) noexcept;
    void set_far(float) noexcept;
    void set_yfov(float) noexcept;

private:
    /// It must be changed by Builder or Manager, because the other components must know about the change
    void set_target(std::shared_ptr<texture::Target>&&) noexcept;
};
}
#endif
