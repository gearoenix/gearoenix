#ifndef GEAROENIX_CORE_EVENT_SYSTEM_SYSTEM_HPP
#define GEAROENIX_CORE_EVENT_SYSTEM_SYSTEM_HPP

#include "cr-ev-event.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        namespace system {
            class System : public Event {
            public:
                class Action {
                public:
                    typedef enum : core::Id {
                        PAUSE = 1,
                        QUIT = 2,
                        RELOAD = 3,
                        RESUME = 4,
                        UNLOAD = 5,
                        UNDEFINED = (core::Id)-1,
                    } Type;
                };

            private:
                const Action::Type action_type;

            protected:
            public:
                System(Action::Type t);
                virtual ~System();
                Action::Type get_action() const;
            };
        }
    }
}
}
#endif
