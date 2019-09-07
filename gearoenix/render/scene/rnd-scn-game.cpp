#include "rnd-scn-game.hpp"

gearoenix::render::scene::Game::Game(
    const core::Id my_id, 
    system::stream::Stream* const f, 
    engine::Engine* const e, 
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Scene(my_id, Type::Game, f, e, c)
{
}

gearoenix::render::scene::Game::Game(
    const core::Id my_id, 
    engine::Engine* const e, 
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Scene(my_id, Type::Game, e, c)
{
}

gearoenix::render::scene::Game::~Game() noexcept
{
}
