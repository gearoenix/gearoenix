#ifndef GEAROEMIX_CORE_EVENT_BUTTON_MOUSE_HPP
#define GEAROEMIX_CORE_EVENT_BUTTON_MOUSE_HPP

#include "cr-ev-bt-button.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        namespace button {
            class Mouse : public Button {
            private:
                const core::Real x;
                const core::Real y;

            public:
                Mouse(KeyType k, core::Real x, core::Real y);
                ~Mouse();
                core::Real get_x() const;
                core::Real get_y() const;
            };
        }
    }
}
}
#endif
