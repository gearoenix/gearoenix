#include "gx-au-manager.hpp"
#include "gx-au-audio.hpp"
#include "gx-au-type.hpp"

gearoenix::audio::Manager::Manager(std::unique_ptr<platform::stream::Stream> s, render::engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

std::shared_ptr<gearoenix::audio::Audio> gearoenix::audio::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Audio>& call) noexcept
{
    std::shared_ptr<Audio> a = cache.get<Audio>(id);
    call.set_data(a);
    return a;
}
