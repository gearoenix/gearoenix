#ifndef GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#define GEAROENIX_RENDER_CAMERA_CAMERA_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"

namespace gearoenix::render::camera {
struct Camera final : public core::ecs::Component {
    GX_GET_CREF_PRV(physics::collider::Frustum, collider)
    GX_GET_CREF_PRV(math::Mat4x4<float>, view)
    GX_GET_CREF_PRV(math::Mat4x4<float>, projection)
    GX_GET_CREF_PRV(math::Mat4x4<float>, view_projection)
    GX_GET_VAL_PRV(std::uint64_t, flag, 1) // Later. scene can use it for models visibility check
    GX_GET_VAL_PRV(float, aspect_ratio, 1.7f)
    GX_GET_VAL_PRV(float, far_plane, 100.0f)
    GX_GET_VAL_PRV(float, near_plane, 1.0f)
    GX_GET_VAL_PRV(float, hdr_tune_mapping, 1.0f)
    GX_GET_VAL_PRV(float, gamma_correction, 2.2f)
    GX_GET_VAL_PRV(bool, is_enabled, true)

    Camera() noexcept;
    ~Camera() noexcept final;
};
}
#endif
