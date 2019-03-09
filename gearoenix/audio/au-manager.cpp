#include "au-manager.hpp"

gearoenix::audio::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<render::engine::Engine>& e)
    : e(e)
    , cache(s)
{
}

gearoenix::audio::Manager::~Manager() {}

std::shared_ptr<gearoenix::audio::Audio> gearoenix::audio::Manager::get(core::Id, core::sync::EndCaller<Audio>)
{
    GXUNIMPLEMENTED;
}
