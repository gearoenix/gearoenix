#include "rnd-cmr-manager.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

gearoenix::render::camera::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e)
    : e(e)
    , cached(s)
{}

gearoenix::render::camera::Manager::~Manager()
{}
