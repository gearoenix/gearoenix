#include "rnd-scn-manager.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../../core/sync/cr-sync-work-waiter.hpp"
#include "rnd-scn-type.hpp"
#include "rnd-scn-scene.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

gearoenix::render::scene::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e)
	: e(e)
	, cache(s)
	, io_worker(new core::sync::WorkWaiter())
{}

gearoenix::render::scene::Manager::~Manager() {}

void gearoenix::render::scene::Manager::get_gx3d(const core::Id mid, core::sync::EndCaller<Scene> c) {
    GXLOGD("Goint to load scene with id: " << mid);
	io_worker->push([mid, c, this] () mutable noexcept -> void {
		c.set_data(cache.get<Scene>(mid, [mid, c, this] () noexcept -> std::shared_ptr<Scene> {
			GXLOGD("Id of scene is " << mid);
			const std::shared_ptr<system::stream::Stream> &file = cache.get_file();
			const core::sync::EndCaller<core::sync::EndCallerIgnore> call(c);
			const Type::Id t = file->read<Type::Id>();
			std::shared_ptr<Scene> s;
			switch (t) {
			case Type::GAME:
				GXLOGD("Type of scene is game.");
                s = std::shared_ptr<Scene>(new Scene(mid, file, e, call));
			case Type::UI:
				GXLOGD("Type of scene is ui.");
				GXUNIMPLEMENTED;
			default:
				GXUNEXPECTED;
			}
			return s;
		}));
	});
}
