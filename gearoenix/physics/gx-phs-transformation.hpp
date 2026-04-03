#pragma once
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../math/gx-math-matrix-3d.hpp"
#include "../math/gx-math-matrix-4d.hpp"
#include "../math/gx-math-quaternion.hpp"
#include "../render/gizmo/gx-rnd-gzm-drawer.hpp"
#include <optional>

namespace gearoenix::physics {
struct Transformation final : core::ecs::Component, render::gizmo::Drawer {
    GEAROENIX_OBJECT_STRUCT_DEF;

    struct Rotation final {
        friend struct Transformation;

        GX_GET_CREF_PRV(math::Quat<core::fp_t>, quat);

        /// This property is for UI purposes only. (it can have other applications as well)
        /// After receiving its initial value, it is updated each frame afterward.
        /// This can negatively impact performance, so use it judiciously.
        /// To avoid unnecessary updates, do not provide an initial value unless strictly required.
        /// For this reason, it is not exposed as a public API.
        /// The value is in degree scale.
        GX_GET_CREF_PRV(std::optional<math::Vec3<core::fp_t>>, euler_in_degree);

    public:
        constexpr Rotation& operator=(const math::Quat<core::fp_t>& q)
        {
            quat = q.normalised();
            if (euler_in_degree.has_value()) {
                euler_in_degree = q.to_euler_degree();
            }
            return *this;
        }

        explicit constexpr operator const math::Quat<core::fp_t>&() const { return quat; }
    };

    constexpr static auto max_count = 8192;
    constexpr static auto object_type_index = gearoenix_physics_transformation_type_index;

    GX_GET_CREF_PRV(math::Mat4x4<core::fp_t>, local_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<core::fp_t>, global_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<core::fp_t>, transposed_inverted_global_matrix);
    GX_GET_CREF_PRV(math::Mat3x3<core::fp_t>, rotation_matrix);
    GX_GET_CREF_PRV(Rotation, rotation);
    GX_GET_CREF_PRV(math::Vec3<core::fp_t>, scale);
    GX_GET_VAL_PRV(bool, changed, true);

    explicit Transformation(core::ecs::Entity* entity, std::string&& name);

public:
    ~Transformation() override;
    void set_local_matrix(const math::Mat4x4<core::fp_t>&);
    [[nodiscard]] math::Vec3<core::fp_t> get_global_position() const;
    void get_global_position(math::Vec3<core::fp_t>&) const;
    void set_local_position(const math::Vec3<core::fp_t>&);
    [[nodiscard]] math::Vec3<core::fp_t> get_local_position() const;
    void get_local_position(math::Vec3<core::fp_t>&) const;
    void local_translate(const math::Vec3<core::fp_t>&);
    void local_x_translate(core::fp_t);
    void local_y_translate(core::fp_t);
    void local_z_translate(core::fp_t);
    void local_outer_rotate(core::fp_t rad, const math::Vec3<core::fp_t>& axis, const math::Vec3<core::fp_t>& position);
    void local_outer_rotate(core::fp_t rad, const math::Vec3<core::fp_t>& axis);
    void local_inner_rotate(core::fp_t rad, const math::Vec3<core::fp_t>& axis);
    void local_inner_rotate(const math::Quat<core::fp_t>& q);
    void local_inner_x_rotate(core::fp_t rad);
    void local_inner_y_rotate(core::fp_t rad);
    void local_inner_z_rotate(core::fp_t rad);
    void set_local_inner_scale(const math::Vec3<core::fp_t>& s);
    void local_inner_scale(core::fp_t s);
    void local_inner_scale(const math::Vec3<core::fp_t>& s);
    void local_inner_x_scale(core::fp_t s);
    void local_inner_y_scale(core::fp_t s);
    void local_inner_z_scale(core::fp_t s);
    void set_rotation(const math::Quat<core::fp_t>&);
    void set_rotation(const Rotation&);
    [[nodiscard]] const math::Vec3<core::fp_t>& get_x_axis() const;
    [[nodiscard]] const math::Vec3<core::fp_t>& get_y_axis() const;
    [[nodiscard]] const math::Vec3<core::fp_t>& get_z_axis() const;
    void local_look_at(const math::Vec3<core::fp_t>& pos, const math::Vec3<core::fp_t>& target, const math::Vec3<core::fp_t>& up);
    void local_look_at(const math::Vec3<core::fp_t>& target, const math::Vec3<core::fp_t>& up);
    void update_without_inverse_root();
    void update_inverse();
    void update_without_inverse_child();
    void clear_change();
    void reset();
    void reset(const math::Vec3<core::fp_t>& s, const Rotation& r, const math::Vec3<core::fp_t>& p);
    void reset(const math::Vec3<core::fp_t>& s, const math::Quat<core::fp_t>& r, const math::Vec3<core::fp_t>& p);
    void reset(const math::Vec3<core::fp_t>& s, const math::Vec3<core::fp_t>& x, const math::Vec3<core::fp_t>& y, const math::Vec3<core::fp_t>& z, const math::Vec3<core::fp_t>& p);
    void show_debug_gui() override;
    void draw_gizmo() override;
    static void update();
};
}
