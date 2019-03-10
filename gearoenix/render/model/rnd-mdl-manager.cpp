#include "rnd-mdl-manager.hpp"

gearoenix::render::model::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e) :
	e(e),
	cache(s)
{}

gearoenix::render::model::Manager::~Manager() {}