#include "rnd-txt-manager.hpp"

gearoenix::render::texture::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e) :
	e(e),
	s(s)
{}

gearoenix::render::texture::Manager::~Manager() {}