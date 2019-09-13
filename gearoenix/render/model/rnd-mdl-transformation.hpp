#ifndef GEAROENIX_RENDER_MODEL_TRANSFORMATION_HPP
#define GEAROENIX_RENDER_MODEL_TRANSFORMATION_HPP
#include "../../physics/phs-transformation.hpp"

namespace gearoenix::math {
struct Sphere;
}

namespace gearoenix::render::model {
struct Uniform;
class Model;
class Transformation : public physics::Transformation {
private:
    Uniform* const uniform;
    math::Sphere* const sphere;
    Model* const parent;

public:
    Transformation(Uniform* uniform, math::Sphere* sphere, Model* parent) noexcept;
    // physics::Transformation----------------------------------------------------------------------------
    //const math::Vec3& get_x_axis() const noexcept override final;
    //const math::Vec3& get_y_axis() const noexcept override final;
    //const math::Vec3& get_z_axis() const noexcept override final;
    const math::Vec3& get_location() const noexcept final;
    //void get_location(math::Vec3& l) const noexcept final;
    void set_location(const math::Vec3& l) noexcept final;
    //void translate(const math::Vec3& t) noexcept override final;
    //void local_x_translate(const core::Real t) noexcept override final;
    //void local_y_translate(const core::Real t) noexcept override final;
    //void local_z_translate(const core::Real t) noexcept override final;
    //void global_rotate(const core::Real d, const math::Vec3& axis, const math::Vec3& location) noexcept override final;
    //void global_rotate(const core::Real d, const math::Vec3& axis) noexcept override final;
    //void local_rotate(const core::Real d, const math::Vec3& axis) noexcept override final;
    //void local_x_rotate(const core::Real d) noexcept override final;
    //void local_y_rotate(const core::Real d) noexcept override final;
    //void local_z_rotate(const core::Real d) noexcept override final;
    //void set_orientation(const math::Quat& q) noexcept override final;
    void local_scale(core::Real) noexcept final;
    void local_x_scale(core::Real) noexcept final;
};
}
#endif
