#ifndef GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#define GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#include "../model/rnd-mdl-model.hpp"

namespace gearoenix {
namespace render {
    namespace widget {
        class Widget : public model::Model {
        public:
            enum Type : core::Id {
                BUTTON = 1,
                EDIT = 2,
                TEXT = 3,
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

        protected:
            StateType state_type = StateType::NORMAL;
            // at the end it must send event
            virtual void press_effect();
            virtual void release_effect();
            virtual void cancel_effect();

        public:
            Widget(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& s,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            Widget(
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            virtual ~Widget();
            void read_gx3d(
                const std::shared_ptr<system::stream::Stream>& f,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            void state_change(EventType e);
        };
    }
}
}
#endif
