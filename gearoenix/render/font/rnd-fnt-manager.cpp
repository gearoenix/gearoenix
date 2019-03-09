#include "rnd-fnt-manager.hpp"

gearoenix::render::font::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e) :
	e(e),
	s(s)
{}

gearoenix::render::font::Manager::~Manager() {}