#ifndef GEAROENIX_PHYSICS_UNTRANSFORMABLE_HPP
#define GEAROENIX_PHYSICS_UNTRANSFORMABLE_HPP
#include "../math/math-vector.hpp"
namespace gearoenix {
namespace physics {
    class Untransferable {
    private:
    protected:
        const math::Vec3 x_axis;
        const math::Vec3 y_axis;
        const math::Vec3 z_axis;
        // it should be call once per object
        void initialize_axis(const math::Mat4x4& t) const;

    public:
        Untransferable();
        virtual ~Untransferable();
        const math::Vec3& get_x_axis() const;
        const math::Vec3& get_y_axis() const;
        const math::Vec3& get_z_axis() const;
        virtual void get_location(math::Vec3& l) const = 0;
        virtual math::Vec3 get_location() const;
    };
}
}
#endif