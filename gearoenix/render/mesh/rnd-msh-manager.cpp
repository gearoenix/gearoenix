#include "rnd-msh-manager.hpp"

gearoenix::render::mesh::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e):
	e(e),
	cache(s)
{}

gearoenix::render::mesh::Manager::~Manager(){}