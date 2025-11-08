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

        GX_GET_CREF_PRV(math::Quat<double>, quat);

        /// This property is for UI purposes only. (it can have other applications as well)
        /// After receiving its initial value, it is updated each frame afterward.
        /// This can negatively impact performance, so use it judiciously.
        /// To avoid unnecessary updates, do not provide an initial value unless strictly required.
        /// For this reason, it is not exposed as a public API.
        /// The value is in degree scale.
        GX_GET_CREF_PRV(std::optional<math::Vec3<double>>, euler_in_degree);

    public:
        constexpr Rotation& operator=(const math::Quat<double>& q)
        {
            quat = q.normalised();
            if (euler_in_degree.has_value()) {
                euler_in_degree = q.to_euler_degree();
            }
            return *this;
        }

        explicit constexpr operator const math::Quat<double>&() const
        {
            return quat;
        }
    };

    constexpr static auto max_count = 8192;
    constexpr static auto object_type_index = gearoenix_physics_transformation_type_index;

    GX_GET_CREF_PRV(math::Mat4x4<double>, local_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<double>, global_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<double>, transposed_inverted_global_matrix);
    GX_GET_CREF_PRV(math::Mat3x3<double>, rotation_matrix);
    GX_GET_CREF_PRV(Rotation, rotation);
    GX_GET_CREF_PRV(math::Vec3<double>, scale);
    GX_GET_VAL_PRV(bool, changed, true);

    explicit Transformation(core::ecs::Entity* entity, std::string&& name);

public:
    ~Transformation() override;
    void set_local_matrix(const math::Mat4x4<double>&);
    [[nodiscard]] math::Vec3<double> get_global_position() const;
    void get_global_position(math::Vec3<double>&) const;
    void set_local_position(const math::Vec3<double>&);
    [[nodiscard]] math::Vec3<double> get_local_position() const;
    void get_local_position(math::Vec3<double>&) const;
    void local_translate(const math::Vec3<double>&);
    void local_x_translate(double);
    void local_y_translate(double);
    void local_z_translate(double);
    void local_outer_rotate(double rad, const math::Vec3<double>& axis, const math::Vec3<double>& position);
    void local_outer_rotate(double rad, const math::Vec3<double>& axis);
    void local_inner_rotate(double rad, const math::Vec3<double>& axis);
    void local_inner_rotate(const math::Quat<double>& q);
    void local_inner_x_rotate(double rad);
    void local_inner_y_rotate(double rad);
    void local_inner_z_rotate(double rad);
    void set_local_inner_scale(const math::Vec3<double>& s);
    void local_inner_scale(double s);
    void local_inner_scale(const math::Vec3<double>& s);
    void local_inner_x_scale(double s);
    void local_inner_y_scale(double s);
    void local_inner_z_scale(double s);
    void set_rotation(const math::Quat<double>&);
    void set_rotation(const Rotation&);
    [[nodiscard]] const math::Vec3<double>& get_x_axis() const;
    [[nodiscard]] const math::Vec3<double>& get_y_axis() const;
    [[nodiscard]] const math::Vec3<double>& get_z_axis() const;
    void local_look_at(const math::Vec3<double>& pos, const math::Vec3<double>& target, const math::Vec3<double>& up);
    void local_look_at(const math::Vec3<double>& target, const math::Vec3<double>& up);
    void update_without_inverse_root();
    void update_inverse();
    void update_without_inverse_child();
    void clear_change();
    void reset();
    void reset(const math::Vec3<double>& s, const Rotation& r, const math::Vec3<double>& p);
    void reset(const math::Vec3<double>& s, const math::Quat<double>& r, const math::Vec3<double>& p);
    void reset(
        const math::Vec3<double>& s,
        const math::Vec3<double>& x,
        const math::Vec3<double>& y,
        const math::Vec3<double>& z,
        const math::Vec3<double>& p);
    void show_debug_gui() override;
    void draw_gizmo() override;
    static void update();
};
}
