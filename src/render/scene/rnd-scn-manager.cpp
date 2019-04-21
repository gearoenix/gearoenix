#include "rnd-scn-manager.hpp"
#include "../../core/sync/cr-sync-work-waiter.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-scn-scene.hpp"
#include "rnd-scn-type.hpp"

gearoenix::render::scene::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e)
    : e(e)
    , cache(s)
    , io_worker(new core::sync::WorkWaiter())
{
}

gearoenix::render::scene::Manager::~Manager() {}

void gearoenix::render::scene::Manager::get_gx3d(const core::Id mid, core::sync::EndCaller<Scene> c)
{
    GXLOGD("Going to load scene with id: " << mid);
    io_worker->push([mid, c, this]() mutable {
        GXLOGD("In IO thread, loading scene with id: " << mid);
        c.set_data(cache.get<Scene>(mid, [mid, c, this] {
            GXLOGD("Scene with id: " << mid << " is not cached, going to import it.");
            const std::shared_ptr<system::stream::Stream>& file = cache.get_file();
            Scene** scnptr = new Scene*;
            const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c, scnptr] {
                (*scnptr)->enable_rendering();
                delete scnptr;
            });
            const Type::Id t = file->read<Type::Id>();
            switch (t) {
            case Type::GAME: {
                GXLOGD("Type of scene is game.");
                std::shared_ptr<Scene> data(new Scene(mid, file, e, call));
                (*scnptr) = data.get();
                return data;
            }
            case Type::UI:
                GXLOGD("Type of scene is ui.");
                GXUNIMPLEMENTED;
            }
            GXUNEXPECTED;
        }));
    });
}

const std::map<gearoenix::core::Id, std::weak_ptr<gearoenix::render::scene::Scene>>& gearoenix::render::scene::Manager::get_scenes() const
{
    return cache.get_cacher().get_cacheds();
}
