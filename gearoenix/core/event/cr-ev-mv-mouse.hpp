#ifndef GEAROEMIX_CORE_EVENT_MOVEMENT_MOUSE_HPP
#define GEAROEMIX_CORE_EVENT_MOVEMENT_MOUSE_HPP

#include "../cr-types.hpp"
#include "cr-ev-mv-movement.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        namespace movement {
            class Mouse : public Movement {
            private:
                const core::Real x;
                const core::Real y;
                const core::Real pre_x;
                const core::Real pre_y;
                const core::Real dx;
                const core::Real dy;

            public:
                Mouse(core::Real x, core::Real y, core::Real pre_x, core::Real pre_y);
            };
        }
    }
}
}
#endif
