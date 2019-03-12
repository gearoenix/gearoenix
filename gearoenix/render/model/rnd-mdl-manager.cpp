#include "rnd-mdl-manager.hpp"

gearoenix::render::model::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e) :
	e(e),
	cache(s)
{}

gearoenix::render::model::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::model::Model> gearoenix::render::model::Manager::get_gx3d(const core::Id, core::sync::EndCaller<Model> &)
{
   GXUNIMPLEMENTED;
}
