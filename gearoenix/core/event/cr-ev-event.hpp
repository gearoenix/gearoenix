#ifndef GEAROEMIX_CORE_EVENT_EVENT_HPP
#define GEAROEMIX_CORE_EVENT_EVENT_HPP

#include "../cr-types.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        namespace button {
            class Button;
        }
        namespace movement {
            class Movement;
        }
        namespace system {
            class System;
        }
        namespace ui {
            class Ui;
        }
        class WindowResize;
        class Event {
        public:
            class From {
            public:
                typedef enum : core::Id {
                    BUTTON = 1,
                    MOVEMENT = 2,
                    SYSTEM = 3,
                    UI = 4,
                    USER_DEFINED = 5,
                    WINDOW_RESIZE = 6,
                    UNDEFINED = 0XFFFFFFFFFFFFFFFF,
                } Type;
            };

        private:
            const From::Type event_type;

        protected:
            Event(From::Type t);

        public:
            virtual ~Event();
            From::Type get_type() const;
            const button::Button& to_button() const;
            const movement::Movement& to_movement() const;
            const system::System& to_system() const;
            const ui::Ui& to_ui() const;
            const WindowResize& to_window_resize() const;
        };
    }
}
}
#endif
