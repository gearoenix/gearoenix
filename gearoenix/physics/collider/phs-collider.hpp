#ifndef GEAROENIX_PHYSICS_COLLIDER_HPP
#define GEAROENIX_PHYSICS_COLLIDER_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace math {
    struct Mat4x4;
    struct Ray3;
}
namespace system {
namespace file {
    class File;
}
}
namespace physics {
    namespace collider {
        class Collider {
        public:
            enum Type : core::Id {
                GHOST = 1,
                MESH = 2,
            };

        protected:
            const Type t;
            Collider(Type t);
            Collider() = delete;

        public:
            virtual ~Collider();
            static Collider* read(system::file::File* in);
            virtual void update(const math::Mat4x4& m) = 0;
            virtual bool hit(const math::Ray3& r, core::Real& distance_from_origin) const = 0;
            virtual void print() const = 0;
        };
    }
}
}
#endif
