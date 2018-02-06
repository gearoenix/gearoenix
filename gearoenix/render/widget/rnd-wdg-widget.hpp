#ifndef GEAROEMIX_RENDER_WIDGET_WIDGET_HPP
#define GEAROEMIX_RENDER_WIDGET_WIDGET_HPP
#include "../model/rnd-mdl-model.hpp"
namespace gearoenix {
namespace render {
    namespace widget {
        class Widget : public model::Model {
            friend class physics::Kernel;

        public:
            enum Type : core::Id {
                BUTTON = 3,
                EDIT = 5,
                TEXT = 4,
                UNKNOWN = 0XFFFFFFFFFFFFFFFF,
            };

            typedef enum : core::Id {
                NORMAL,
                PRESSED,
            } StateType;

            // This is something internal, it's gonna be used between ui-scene & widget
            typedef enum : core::Id {
                PRESS,
                RELEASE,
                MOVE_OUT,
            } EventType;

        private:
            //std::mutex event_locker;

        protected:
            StateType state_type = StateType::NORMAL;
            Widget(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            // at the end it must send event
            virtual void press_effect();
            virtual void release_effect();
            virtual void cancel_effect();

        public:
            static Widget* read(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            virtual ~Widget();
            void state_change(EventType e, core::Id my_id);
        };
    }
}
}
#endif
