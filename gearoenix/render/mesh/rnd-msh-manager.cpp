#include "rnd-msh-manager.hpp"
#include "rnd-msh-type.hpp"
#include "rnd-msh-mesh.hpp"

gearoenix::render::mesh::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e):
	e(e),
	cache(s)
{}

gearoenix::render::mesh::Manager::~Manager(){}

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::render::mesh::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Mesh> &c) 
{
	auto m = cache.get<Mesh>(id, [id, c, this] {
		const auto &f = cache.get_file();
		switch(f->read<core::TypeId>())
		{
		case Type::BASIC:
			return std::make_shared<Mesh>(id, f, e, c);
		default:
			GXUNEXPECTED;
		}
	});
	c.set_data(m);
	return m;
}