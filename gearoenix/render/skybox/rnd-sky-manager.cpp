#include "rnd-sky-manager.hpp"

gearoenix::render::skybox::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}