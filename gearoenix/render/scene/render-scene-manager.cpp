#include "render-scene-manager.hpp"
#include "../../system/sys-file.hpp"
#include "../render-engine.hpp"
//#include "render-scene.hpp"

gearoenix::render::scene::Manager::Manager(
    Engine* engine, const std::shared_ptr<system::File>& asset)
    : engine(engine)
    , asset(asset)
{
    uint16_t count;
    asset->read(count);
    offsets.resize(count);
    for (uint16_t i = 0; i < count; ++i) {
        asset->read(offsets[i]);
    }
}

gearoenix::render::scene::Manager::~Manager()
{
    cached.clear();
    offsets.clear();
}

std::shared_ptr<gearoenix::render::scene::Scene>& gearoenix::render::scene::Manager::get_scene(uint16_t id)
{
}
