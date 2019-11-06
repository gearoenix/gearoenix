#include "rnd-scn-manager.hpp"
#include "../../core/sync/cr-sync-work-waiter.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-scn-game.hpp"
#include "rnd-scn-type.hpp"
#include "rnd-scn-ui.hpp"

gearoenix::render::scene::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
    , io_worker(new core::sync::WorkWaiter())
{
}

void gearoenix::render::scene::Manager::get_gx3d(const core::Id mid, core::sync::EndCaller<Scene> c) noexcept
{
    GXLOGD("Going to load scene with id: " << mid)
    io_worker->push([mid, c, this]() mutable noexcept {
        GXLOGD("In IO thread, loading scene with id: " << mid)
        c.set_data(cache.get<Scene>(
            mid, [mid, c, this]() noexcept -> std::shared_ptr<Scene> {
                GXLOGD("Scene with id: " << mid << " is not cached, it is going to be imported.")
                system::stream::Stream* const file = cache.get_file();
                const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
                const auto t = file->read<Type>();
                switch (t) {
                case Type::Game:
                    GXLOGD("Type of scene is game.")
                    return std::make_shared<Game>(mid, file, e, call);
                case Type::Ui:
                    GXLOGD("Type of scene is ui.")
                    return std::make_shared<Ui>(mid, file, e, call);
                default:
                    GXLOGF("Unexpected scene type: " << static_cast<core::TypeId>(t))
                }
            }));
    });
}

const std::map<gearoenix::core::Id, std::weak_ptr<gearoenix::render::scene::Scene>>& gearoenix::render::scene::Manager::get_scenes() const noexcept
{
    return cache.get_cacher().get_cacheds();
}
