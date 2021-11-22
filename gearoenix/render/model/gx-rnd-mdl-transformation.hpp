#ifndef GEAROENIX_RENDER_MODEL_TRANSFORMATION_HPP
#define GEAROENIX_RENDER_MODEL_TRANSFORMATION_HPP
#include "../../physics/gx-phs-transformation.hpp"

namespace gearoenix::render::model {
struct Uniform;
struct Model;
struct Transformation : public physics::Transformation {
private:
    Model* const parent;

public:
    explicit Transformation(Model* parent) noexcept;
    // physics::Transformation----------------------------------------------------------------------------
    // const math::Vec3& get_x_axis() const noexcept override final;
    // const math::Vec3& get_y_axis() const noexcept override final;
    // const math::Vec3& get_z_axis() const noexcept override final;
    [[nodiscard]] math::Vec3<double> get_location() const noexcept final;
    // void get_location(math::Vec3& l) const noexcept final;
    void set_location(const math::Vec3<double>& l) noexcept final;
    void translate(const math::Vec3<double>& t) noexcept final;
    // void local_x_translate(const double t) noexcept override final;
    // void local_y_translate(const double t) noexcept override final;
    // void local_z_translate(const double t) noexcept override final;
    // void global_rotate(const double d, const math::Vec3& axis, const math::Vec3& location) noexcept override final;
    // void global_rotate(const double d, const math::Vec3& axis) noexcept override final;
    // void local_rotate(const double d, const math::Vec3& axis) noexcept override final;
    // void local_x_rotate(const double d) noexcept override final;
    // void local_y_rotate(const double d) noexcept override final;
    void local_z_rotate(double d) noexcept final;
    // void set_orientation(const math::Quat& q) noexcept override final;
    void local_scale(double) noexcept final;
    void local_scale(const math::Vec3<double>& s) noexcept final;
    void local_x_scale(double) noexcept final;
    void local_y_scale(double) noexcept final;
    void local_z_scale(double) noexcept final;
};
}
#endif
