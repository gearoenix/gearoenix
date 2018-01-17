#ifndef GEAROENIX_PHYSICS_COLLIDER_HPP
#define GEAROENIX_PHYSICS_COLLIDER_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace math {
    struct Mat4x4;
}
namespace system {
    class File;
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
            static Collider* read(system::File* in);
            virtual void update(const math::Mat4x4& m) = 0;
        };
    }
}
}
#endif
