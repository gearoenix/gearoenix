#ifndef GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#define GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../math/gx-math-matrix-4d.hpp"
#include "../math/gx-math-quaternion.hpp"
#include <functional>

namespace gearoenix::physics {
struct Transformation {
    GX_GET_CREF_PRV(math::Mat4x4<double>, local_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<double>, global_matrix);
    /// This is useful for caching the calculation
    /// It gets updated in each loop so be careful and check the `changed` variable
    GX_GET_CREF_PRV(math::Mat4x4<double>, inverted_global_matrix);
    GX_GET_CREF_PRV(math::Vec3<double>, x_axis);
    GX_GET_CREF_PRV(math::Vec3<double>, y_axis);
    GX_GET_CREF_PRV(math::Vec3<double>, z_axis);
    GX_GET_CREF_PRV(math::Vec3<double>, scale);
    GX_GET_VAL_PRV(bool, changed, true);

public:
    Transformation();
    Transformation(Transformation&&) = default;
    Transformation& operator=(Transformation&&) = default;
    virtual ~Transformation() = default;
    [[nodiscard]] math::Vec3<double> get_global_location() const;
    void get_global_location(math::Vec3<double>& l) const;
    [[nodiscard]] math::Vec3<double> get_local_location() const;
    void get_local_location(math::Vec3<double>& l) const;
    void set_local_location(const math::Vec3<double>& l);
    void local_translate(const math::Vec3<double>& t);
    void local_x_translate(double v);
    void local_y_translate(double v);
    void local_z_translate(double v);
    void local_outer_rotate(double d, const math::Vec3<double>& axis, const math::Vec3<double>& location);
    void local_outer_rotate(double d, const math::Vec3<double>& axis);
    void local_rotate(double d, const math::Vec3<double>& axis);
    void local_rotate(const math::Quat<double>& q);
    void local_x_rotate(double d);
    void local_y_rotate(double d);
    void local_z_rotate(double d);
    void set_local_scale(const math::Vec3<double>& s);
    void local_scale(double s);
    void local_scale(const math::Vec3<double>& s);
    void local_x_scale(double s);
    void local_y_scale(double s);
    void local_z_scale(double s);
    void set_local_orientation(const math::Quat<double>& q);
    [[nodiscard]] math::Quat<double> get_local_orientation() const;
    void local_look_at(const math::Vec3<double>& location, const math::Vec3<double>& target, const math::Vec3<double>& up);
    void local_look_at(const math::Vec3<double>& target, const math::Vec3<double>& up);
    void local_update();
    void update(const Transformation& parent);
    void local_update_without_inverse();
    void update_without_inverse(const Transformation& parent);
    void clear_change();
    void reset();
    void reset(const math::Vec3<double>& scale, const math::Quat<double>& rotation, const math::Vec3<double>& location);
    void reset(
        const math::Vec3<double>& scale,
        const math::Vec3<double>& x_axis,
        const math::Vec3<double>& y_axis,
        const math::Vec3<double>& z_axis,
        const math::Vec3<double>& location);
    void show_debug_gui();
};

struct TransformationComponent final : core::ecs::Component, Transformation {
private:
    [[nodiscard]] const boost::container::flat_set<std::type_index>& get_all_the_hierarchy_types_except_component() const override;

public:
    explicit TransformationComponent(std::string&& name);
    [[nodiscard]] static std::shared_ptr<TransformationComponent> construct(std::string&& name);
};
}
#endif
