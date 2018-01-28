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

        protected:
            Widget(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);

        public:
            static Widget* read(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            virtual ~Widget();
        };
    }
}
}
#endif
