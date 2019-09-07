#ifndef GEAROENIX_RENDER_SCENE_GAME_HPP
#define GEAROENIX_RENDER_SCENE_GAME_HPP
#include "rnd-scn-scene.hpp"

namespace gearoenix::render::scene {
class Game : public Scene {
public:
    Game(
        core::Id my_id, system::stream::Stream* f, engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Game(
        core::Id my_id, engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Game() noexcept;
};
}
#endif
