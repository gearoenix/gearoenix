#ifndef GEAROEMIX_RENDER_WIDGET_TEXT_HPP
#define GEAROEMIX_RENDER_WIDGET_TEXT_HPP
#include "rnd-wdg-widget.hpp"
namespace gearoenix {
namespace render {
    namespace widget {
        class Text : public Widget {
            friend class physics::Kernel;

        public:
        protected:
        public:
            Text(system::stream::Stream* s, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            ~Text();
        };
    }
}
}
#endif
