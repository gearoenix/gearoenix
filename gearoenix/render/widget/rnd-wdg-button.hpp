#ifndef GEAROEMIX_RENDER_WIDGET_BUTTON_HPP
#define GEAROEMIX_RENDER_WIDGET_BUTTON_HPP
#include "rnd-wdg-widget.hpp"
namespace gearoenix {
namespace render {
    namespace widget {
        class Button : public Widget {
            friend class physics::Kernel;

        public:
            Button(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            virtual ~Button();
        };
    }
}
}
#endif
