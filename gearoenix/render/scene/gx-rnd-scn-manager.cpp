#include "gx-rnd-scn-manager.hpp"
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-scn-game.hpp"
#include "gx-rnd-scn-type.hpp"
#include "gx-rnd-scn-ui.hpp"

gearoenix::render::scene::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
    , io_worker(new core::sync::WorkWaiter())
{
}

gearoenix::render::scene::Manager::~Manager() noexcept = default;

void gearoenix::render::scene::Manager::get_gx3d(const core::Id mid, core::sync::EndCaller<Scene> c) noexcept
{
    GXLOGD("Going to load scene with id: " << mid)
    io_worker->push([mid, c, this]() mutable noexcept {
        GXLOGD("In IO thread, loading scene with id: " << mid)
        c.set_data(cache.get<Scene>(
            mid, [mid, c, this](std::string name) noexcept {
                GXLOGD("Scene with id: " << mid << " is not cached, it is going to be imported.")
                system::stream::Stream* const file = cache.get_file();
                const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
                const auto t = file->read<Type>();
                switch (t) {
                case Type::Game:
                    GXLOGD("Type of scene is game.")
                    return std::shared_ptr<Scene>(new Game(mid, std::move(name), file, e, call));
                case Type::Ui:
                    GXLOGD("Type of scene is ui.")
                    return std::shared_ptr<Scene>(new Ui(mid, std::move(name), file, e, call));
                default:
                    GXLOGF("Unexpected scene type: " << static_cast<core::TypeId>(t))
                }
            }));
    });
}

void gearoenix::render::scene::Manager::get_gx3d(const std::string& name, const core::sync::EndCaller<Scene>& c) noexcept
{
    get_gx3d(cache.get_cacher().get_key(name), c);
}

const std::map<gearoenix::core::Id, std::weak_ptr<gearoenix::render::scene::Scene>>& gearoenix::render::scene::Manager::get_scenes() const noexcept
{
    return cache.get_cacher().get_cacheds();
}
