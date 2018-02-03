#ifndef GEAROEMIX_CORE_EVENT_BUTTON_BUTTON_HPP
#define GEAROEMIX_CORE_EVENT_BUTTON_BUTTON_HPP

#include "cr-ev-event.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        namespace button {
            class Mouse;
            class Keyboard;
            class Joystick;
            class Button : public Event {
            public:
                typedef enum : core::Id {
                    MOUSE,
                    KEYBOARD,
                    JOYSTICK
                    // todo others will added when needed
                } ButtonType;
                // these are general keys, it only gets any meaning when button type considered
                typedef enum : core::Id {
                    LEFT,
                    RIGHT,
                    MIDDLE,
                    UP,
                    DOWN,
                    FRONT,
                    BACK,
                    ESCAPE,
                    SELECT,
                    START,
                    A,
                    B,
                    X,
                    Y,
                    R1,
                    R2,
                    L1,
                    L2,
                    // todo others will added when needed
                } KeyType;
                typedef enum : core::Id {
                    PRESS,
                    RELEASE,
                    DOUBLE,
                } ActionType;

            private:
                const ButtonType button_type;
                const KeyType key_type;
                const ActionType action_type;

            protected:
                Button(ButtonType t, KeyType k, ActionType a);

            public:
                virtual ~Button();
                ButtonType get_type() const;
                KeyType get_key() const;
                ActionType get_action() const;
                const Mouse& to_mouse() const;
            };
        }
    }
}
}
#endif
