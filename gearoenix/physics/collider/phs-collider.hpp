#ifndef GEAROENIX_PHYSICS_COLLIDER_HPP
#define GEAROENIX_PHYSICS_COLLIDER_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace math {
    struct Mat4x4;
    struct Ray3;
}
namespace system::stream {
    class Stream;
}
namespace physics::collider {
    class Collider {
    public:
        enum Type : core::Id {
            GHOST = 1,
            MESH = 2,
        };

    protected:
        const Type t;
        explicit Collider(Type t) noexcept;

    public:
        Collider() = delete;
        virtual ~Collider() noexcept = default;
        static Collider* read(system::stream::Stream* f)  noexcept;
        virtual void update(const math::Mat4x4& m) noexcept = 0;
        virtual bool hit(const math::Ray3& r, core::Real& distance_from_origin) const  noexcept = 0;
    };
}
}
#endif
