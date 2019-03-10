#include "rnd-scn-manager.hpp"

gearoenix::render::scene::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e) :
	e(e),
	cache(s)
{}

gearoenix::render::scene::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::scene::Scene> gearoenix::render::scene::Manager::get(const core::Id mid, const core::sync::EndCaller<Scene> c) {
	GXUNIMPLEMENTED;
	return nullptr;
}