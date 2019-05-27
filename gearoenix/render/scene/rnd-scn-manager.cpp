#include "rnd-scn-manager.hpp"
#include "../../core/sync/cr-sync-work-waiter.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-scn-scene.hpp"
#include "rnd-scn-type.hpp"
#include <utility>

gearoenix::render::scene::Manager::Manager(system::stream::Stream*const s, engine::Engine*const e) noexcept
        : e(e)
    , cache(s)
    , io_worker(new core::sync::WorkWaiter())
{
}

void gearoenix::render::scene::Manager::get_gx3d(const core::Id mid, core::sync::EndCaller<Scene> c) noexcept
{
    GXLOGD("Going to load scene with id: " << mid)
    io_worker->push([mid, c, this]() mutable {
        GXLOGD("In IO thread, loading scene with id: " << mid)
        c.set_data(cache.get<Scene>(mid, [mid, c, this] {
            GXLOGD("Scene with id: " << mid << " is not cached, going to import it.")
            const std::shared_ptr<system::stream::Stream>& file = cache.get_file();
            auto scnptr = new Scene*;
            const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c, scnptr] {
                (*scnptr)->enable();
                delete scnptr;
            });
            const auto t = file->read<Type::Id>();
            switch (t) {
            case Type::GAME: {
                GXLOGD("Type of scene is game.")
                std::shared_ptr<Scene> data(new Scene(mid, file, e, call));
                (*scnptr) = data.get();
                return data;
            }
            case Type::UI:
                GXLOGD("Type of scene is ui.")
                GXUNIMPLEMENTED
            }
            GXUNEXPECTED
        }));
    });
}

const std::map<gearoenix::core::Id, std::weak_ptr<gearoenix::render::scene::Scene>>& gearoenix::render::scene::Manager::get_scenes() const noexcept
{
    return cache.get_cacher().get_cacheds();
}
