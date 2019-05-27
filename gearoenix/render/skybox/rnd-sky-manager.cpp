#include "rnd-sky-manager.hpp"

gearoenix::render::skybox::Manager::Manager(system::stream::Stream*const s, engine::Engine*const e) noexcept
    : e(e)
    , cache(s)
{
}