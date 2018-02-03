#ifndef GEAROEMIX_RENDER_SCENE_UI_HPP
#define GEAROEMIX_RENDER_SCENE_UI_HPP
#include "rnd-scn-scene.hpp"

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
            std::pair<std::shared_ptr<widget::Widget>, std::shared_ptr<physics::animation::Animation>> pressed;
            Ui(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            std::shared_ptr<widget::Widget> find_widget_under_cursor(core::Real x, core::Real y);

        public:
            virtual void on_event(const core::event::Event& e);
        };
    }
}
}
#endif
