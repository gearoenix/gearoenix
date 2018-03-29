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
            friend class physics::Kernel;
            friend class Scene;

        protected:
            core::Id pressed = (core::Id)-1;
            core::Id mouse_overed = (core::Id)-1;
            Ui(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);

        public:
            virtual void on_event(core::event::Event& e);
            core::Id find_widget_under_cursor(core::Real x, core::Real y);
        };
    }
}
}
#endif
