#include "gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-cmr-orthographic.hpp"
#include "gx-rnd-cmr-perspective.hpp"

gearoenix::render::camera::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

std::shared_ptr<gearoenix::render::camera::Camera> gearoenix::render::camera::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Camera>& call) noexcept
{
    std::shared_ptr<Camera> data = cache.get<Camera>(
        id, [this, id, call](std::string name) noexcept -> std::shared_ptr<Camera> {
            system::stream::Stream* const file = cache.get_file();
            const auto t = file->read<core::TypeId>();
            switch (t) {
            case 1:
                return std::make_shared<Perspective>(id, std::move(name), file, e);
            case 2:
                return std::make_shared<Orthographic>(id, std::move(name), file, e);
            default:
                GXLOGF("Unexpected camera type " << t)
            }
        });
    call.set_data(data);
    return data;
}
