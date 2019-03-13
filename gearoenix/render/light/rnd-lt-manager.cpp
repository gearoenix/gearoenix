#include "rnd-lt-manager.hpp"
#include "rnd-lt-type.hpp"
#include "rnd-lt-directional.hpp"

gearoenix::render::light::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e)
	: e(e)
    , cache(s)
{
}

gearoenix::render::light::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::light::Light> gearoenix::render::light::Manager::get_gx3d(
    const core::Id id, core::sync::EndCaller<Light> &call)
{
	std::shared_ptr<Light> l = cache.get<Light>(id, [id, call, this] {
		const std::shared_ptr<system::stream::Stream> &f = cache.get_file();
		const Type::Id t = f->read<Type::Id>();
		switch (t)
		{
		case Type::CONE:
			GXUNIMPLEMENTED;
		case Type::POINT:
			GXUNIMPLEMENTED;
		case Type::DIRECTIONAL:
			return std::make_shared<Directional>(id, f, e);
		default:
			GXUNEXPECTED;
		}
	});
	call.set_data(l);
	return l;
}
