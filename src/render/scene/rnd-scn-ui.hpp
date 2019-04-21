#ifndef GEAROEMIX_RENDER_SCENE_UI_HPP
#define GEAROEMIX_RENDER_SCENE_UI_HPP
#include "rnd-scn-scene.hpp"
#include <chrono>

namespace gearoenix {
namespace physics {
    namespace animation {
        class Animation;
    }
}
namespace render {
    namespace widget {
        class Widget;
    }
    namespace scene {
        class Ui : public Scene {

        protected:
            core::Id pressed = 0;
            core::Id mouse_overed = 0;

        public:
            // virtual void on_event(core::event::Event& e);
            // core::Id find_widget_under_cursor(core::Real x, core::Real y);
        };
    }
}
}
#endif
