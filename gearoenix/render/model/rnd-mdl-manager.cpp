#include "rnd-mdl-manager.hpp"
#include "rnd-mdl-model.hpp"
#include "rnd-mdl-type.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

gearoenix::render::model::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e) :
	e(e),
	cache(s)
{}

gearoenix::render::model::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::model::Model> gearoenix::render::model::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Model> &c)
{
	std::shared_ptr<Model> m = cache.get<Model>(id, [id, c, this] {
		const std::shared_ptr<system::stream::Stream> &f = cache.get_file();
		const Type::Id t = f->read<Type::Id>();
		const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
		switch (t)
		{
		case Type::DYNAMIC:
		case Type::STATIC:
			return std::make_shared<Model>(id, f, e, call, Type::DYNAMIC == t);
		case Type::WIDGET:
			GXUNIMPLEMENTED;
		default:
			GXUNEXPECTED;
		}
	});
	c.set_data(m);
	return m;
}
