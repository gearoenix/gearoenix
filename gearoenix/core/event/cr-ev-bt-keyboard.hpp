#ifndef GEAROEMIX_CORE_EVENT_BUTTON_KEYBOARD_HPP
#define GEAROEMIX_CORE_EVENT_BUTTON_KEYBOARD_HPP

#include "cr-ev-bt-button.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        namespace button {
            class Keyboard : public Button {
            private:

            public:
				Keyboard(KeyType k, ActionType a);
                virtual ~Keyboard();
            };
        }
    }
}
}
#endif
