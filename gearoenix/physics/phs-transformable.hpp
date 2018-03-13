#ifndef GEAROENIX_PHYSICS_TRANSFORMABLE_HPP
#define GEAROENIX_PHYSICS_TRANSFORMABLE_HPP
#include "../math/math-vector.hpp"
namespace gearoenix {
namespace physics {
    class Transferable {
    private:
    protected:
        bool transformed = true;
        math::Vec3 x_axis, y_axis, z_axis;
        void local_x_rotate(const core::Real d, math::Mat4x4& rm);
        void local_y_rotate(const core::Real d, math::Mat4x4& rm);
        void local_z_rotate(const core::Real d, math::Mat4x4& rm);
        void local_rotate(const core::Real d, const math::Vec3& a, math::Mat4x4& rm);
        void initialize_axis(const math::Mat4x4& t);

    public:
        Transferable();
        virtual ~Transferable();
        const math::Vec3& get_x_axis() const;
        const math::Vec3& get_y_axis() const;
        const math::Vec3& get_z_axis() const;
        bool is_transformed() const;
        virtual void commit_transformation();
        virtual void get_location(math::Vec3& l) const = 0;
        virtual void set_location(const math::Vec3& l) = 0;
        virtual void translate(const math::Vec3& t) = 0;
        virtual void local_x_translate(const core::Real t);
        virtual void local_y_translate(const core::Real t);
        virtual void local_z_translate(const core::Real t);
        virtual void global_rotate(const core::Real d, const math::Vec3& axis, const math::Vec3& location);
        virtual void global_rotate(const core::Real d, const math::Vec3& axis) = 0;
        virtual void global_rotate(const math::Mat4x4& rm);
        virtual void local_rotate(const core::Real d, const math::Vec3& axis) = 0;
        virtual void local_x_rotate(const core::Real d) = 0;
        virtual void local_y_rotate(const core::Real d) = 0;
        virtual void local_z_rotate(const core::Real d) = 0;
        virtual void global_scale(const core::Real s) = 0;
        virtual void local_scale(const core::Real s) = 0;
    };
}
}
#endif
