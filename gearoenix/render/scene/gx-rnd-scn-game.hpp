#ifndef GEAROENIX_RENDER_SCENE_GAME_HPP
#define GEAROENIX_RENDER_SCENE_GAME_HPP
#include "gx-rnd-scn-scene.hpp"

namespace gearoenix::render::scene {
class Game final : public Scene {
public:
    Game(
        core::Id my_id, std::string name, system::stream::Stream* f, engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Game(
        core::Id my_id, std::string name, engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Game() noexcept final;
};
}
#endif
