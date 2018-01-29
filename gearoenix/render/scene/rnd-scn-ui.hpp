#ifndef GEAROEMIX_RENDER_SCENE_UI_HPP
#define GEAROEMIX_RENDER_SCENE_UI_HPP
#include "rnd-scn-scene.hpp"

namespace gearoenix {
namespace render {
    namespace scene {
        class Ui : public Scene {
            friend class physics::Kernel;
            friend class Scene;

        protected:
            Ui(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);

        public:
            virtual void on_event(const core::event::Event& e);
        };
    }
}
}
#endif
