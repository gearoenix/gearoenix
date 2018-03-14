#ifndef GEAROENIX_PHYSICS_BODY_STATIC_HPP
#define GEAROENIX_PHYSICS_BODY_STATIC_HPP
#include "../../math/math-vector.hpp"
#include "phs-bd-body.hpp"
namespace gearoenix {
namespace physics {
    namespace body {
        class Static : public Body {
        private:
        protected:
            core::Real friction;
            core::Real bounciness;

        public:
            Static();
            virtual ~Static();
        };
    }
}
}
#endif
