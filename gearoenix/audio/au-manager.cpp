#include "au-manager.hpp"
#include "au-audio.hpp"
#include "au-type.hpp"

gearoenix::audio::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<render::engine::Engine>& e)
    : e(e)
    , cache(s)
{
}

gearoenix::audio::Manager::~Manager() {}

std::shared_ptr<gearoenix::audio::Audio> gearoenix::audio::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Audio>& call)
{
    std::shared_ptr<Audio> a = cache.get<Audio>(id);
    call.set_data(a);
    return a;
}
