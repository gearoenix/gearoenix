#ifndef GEAROENIX_RENDER_SCENE_UI_HPP
#define GEAROENIX_RENDER_SCENE_UI_HPP
#include "rnd-scn-scene.hpp"
#include <chrono>

namespace gearoenix::render::scene {
class Ui : public Scene {
protected:
public:
    Ui(
        const core::Id my_id, system::stream::Stream* f, engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Ui(
        const core::Id my_id, engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
};
}
#endif
