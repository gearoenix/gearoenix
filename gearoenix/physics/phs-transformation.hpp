#ifndef GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#define GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#include "../math/math-matrix.hpp"
#include "../math/math-quaternion.hpp"
#include "../math/math-vector.hpp"
namespace gearoenix {
namespace physics {
    class Transformation {
    protected:
        Transformation() noexcept;

    public:
        virtual ~Transformation() noexcept;
        virtual const math::Vec3& get_x_axis() const noexcept;
        virtual const math::Vec3& get_y_axis() const noexcept;
        virtual const math::Vec3& get_z_axis() const noexcept;
        virtual const math::Vec3& get_location() const noexcept;
        virtual void get_location(math::Vec3& l) const noexcept;
        virtual void set_location(const math::Vec3& l) noexcept;
        virtual void translate(const math::Vec3& t) noexcept;
        virtual void local_x_translate(const core::Real t) noexcept;
        virtual void local_y_translate(const core::Real t) noexcept;
        virtual void local_z_translate(const core::Real t) noexcept;
        virtual void global_rotate(const core::Real d, const math::Vec3& axis, const math::Vec3& location) noexcept;
        virtual void global_rotate(const core::Real d, const math::Vec3& axis) noexcept;
        virtual void local_rotate(const core::Real d, const math::Vec3& axis) noexcept;
        virtual void local_x_rotate(const core::Real d) noexcept;
        virtual void local_y_rotate(const core::Real d) noexcept;
        virtual void local_z_rotate(const core::Real d) noexcept;
        virtual void scale(const core::Real s) noexcept;
        virtual void set_orientation(const math::Quat& q) noexcept;
    };
}
}
#endif
