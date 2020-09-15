#include "gx-rnd-lt-manager.hpp"
#include "gx-rnd-lt-directional.hpp"
#include "gx-rnd-lt-point.hpp"
#include "gx-rnd-lt-type.hpp"
#include <memory>

gearoenix::render::light::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

std::shared_ptr<gearoenix::render::light::Light> gearoenix::render::light::Manager::get_gx3d(
    const core::Id id, core::sync::EndCaller<Light>& call) noexcept
{
    auto l = cache.get<Light>(id, [id, call, this](std::string name) {
        system::stream::Stream* const f = cache.get_file();
        const auto t = f->read<Type>();
        switch (t) {
        case Type::Cone:
            GX_UNIMPLEMENTED
        case Type::Point:
            return std::shared_ptr<Light>(new Point(id, std::move(name), f, e));
        case Type::Directional:
            return std::shared_ptr<Light>(new Directional(id, std::move(name), f, e));
        default:
            GX_UNEXPECTED
        }
    });
    call.set_data(l);
    return l;
}
