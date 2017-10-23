#include "rnd-pip-manager.hpp"
#include "../../core/cache/cr-cache-cacher.hpp"
#include "../rnd-engine.hpp"
#include "rnd-pip-pipeline.hpp"

gearoenix::render::pipeline::Manager::Manager(Engine* engine)
    : eng(engine)
    , csh(new core::cache::Cacher)
{
}

gearoenix::render::pipeline::Manager::~Manager()
{
    delete csh;
}

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::render::pipeline::Manager::get_pipeline(core::Id sid, std::shared_ptr<core::EndCaller> end)
{
    std::function<std::shared_ptr<Pipeline>()> f = [this, sid, end]() {
        return std::shared_ptr<Pipeline>(eng->create_pipeline(sid, end));
    };
    return csh->get(sid, f);
}

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::render::pipeline::Manager::get_cached_pipeline(core::Id sid) const
{
    return csh->get<Pipeline>(sid);
}
