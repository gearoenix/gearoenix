#include "rnd-lt-manager.hpp"
#include "rnd-lt-directional.hpp"
#include "rnd-lt-point.hpp"
#include "rnd-lt-type.hpp"
#include <memory>

gearoenix::render::light::Manager::Manager(system::stream::Stream* const s, engine::Engine* const e) noexcept
    : e(e)
    , cache(s)
{
}

std::shared_ptr<gearoenix::render::light::Light> gearoenix::render::light::Manager::get_gx3d(
    const core::Id id, core::sync::EndCaller<Light>& call) noexcept
{
    std::shared_ptr<Light> l = cache.get<Light>(id, [id, call, this] {
        system::stream::Stream* f = cache.get_file();
        const auto t = f->read<Type>();
        switch (t) {
        case Type::CONE:
            GXUNIMPLEMENTED;
        case Type::POINT:
            return std::shared_ptr<Light>(new Point(id, f, e));
        case Type::DIRECTIONAL:
            return std::shared_ptr<Light>(new Directional(id, f, e));
        default:
            GXUNEXPECTED;
        }
    });
    call.set_data(l);
    return l;
}
