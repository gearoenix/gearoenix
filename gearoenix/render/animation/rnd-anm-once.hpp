#include "rnd-anm-animation.hpp"

namespace gearoenix {
namespace render {
    namespace animation {
        class Once {
        protected:
            std::chrono::steady_clock::time_point end;

        public:
            Once(std::chrono::duration<core::Real> duration, math::Transformable* obj);
            virtual ~Once();
        };
    }
}
}
