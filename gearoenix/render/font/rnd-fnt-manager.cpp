#include "rnd-fnt-manager.hpp"

gearoenix::render::font::Manager::Manager(system::stream::Stream*const s, engine::Engine*const e) noexcept
    : e(e)
    , cache(s)
{
}