#include "rnd-sky-manager.hpp"

gearoenix::render::skybox::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e)
    : e(e)
    , cache(s)
{
}

gearoenix::render::skybox::Manager::~Manager() {}