#ifndef GEAROENIX_PHYSICS_TRANSFORMABLE_HPP
#define GEAROENIX_PHYSICS_TRANSFORMABLE_HPP
#include "../math/math-matrix.hpp"
#include "../math/math-vector.hpp"
#include <memory>
namespace gearoenix {
namespace math {
    struct Quat;
}
namespace physics {
    class Transformation {
    private:
    protected:
        math::Vec3 x_axis, y_axis, z_axis;
        math::Mat4x4 transformation;

    public:
        Transformation();
        virtual ~Transformation();
        const math::Vec3& get_x_axis() const;
        const math::Vec3& get_y_axis() const;
        const math::Vec3& get_z_axis() const;
        const math::Mat4x4& get_transformation() const;
        virtual void get_location(math::Vec3& l) const;
        virtual const math::Vec3 get_location() const;
        virtual void set_location(const math::Vec3& l);
        virtual void translate(const math::Vec3& t);
        virtual void local_x_translate(const core::Real t);
        virtual void local_y_translate(const core::Real t);
        virtual void local_z_translate(const core::Real t);
        virtual void global_rotate(const core::Real d, const math::Vec3& axis, const math::Vec3& location);
        virtual void global_rotate(const core::Real d, const math::Vec3& axis);
        virtual void local_rotate(const core::Real d, const math::Vec3& axis);
        virtual void local_x_rotate(const core::Real d);
        virtual void local_y_rotate(const core::Real d);
        virtual void local_z_rotate(const core::Real d);
        virtual void global_scale(const core::Real s);
        virtual void local_scale(const core::Real s);
        /// It cleans all the scale tranformations
        virtual void set_orientation(const math::Quat& q);
    };
}
}
#endif
