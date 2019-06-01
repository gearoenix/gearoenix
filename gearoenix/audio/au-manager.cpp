#include "au-manager.hpp"
#include "au-audio.hpp"
#include "au-type.hpp"

gearoenix::audio::Manager::Manager(system::stream::Stream* const s, render::engine::Engine* const e) noexcept
    : e(e)
    , cache(s)
{
}

std::shared_ptr<gearoenix::audio::Audio> gearoenix::audio::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Audio>& call) noexcept
{
    std::shared_ptr<Audio> a = cache.get<Audio>(id);
    call.set_data(a);
    return a;
}
