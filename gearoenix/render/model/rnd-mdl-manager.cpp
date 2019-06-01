#include "rnd-mdl-manager.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "rnd-mdl-model.hpp"
#include "rnd-mdl-type.hpp"

gearoenix::render::model::Manager::Manager(system::stream::Stream* const s, engine::Engine* const e) noexcept
    : e(e)
    , cache(s)
{
}

std::shared_ptr<gearoenix::render::model::Model> gearoenix::render::model::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Model>& c) noexcept
{
    std::shared_ptr<Model> m = cache.get<Model>(id, [id, c, this] {
        system::stream::Stream* f = cache.get_file();
        const auto t = f->read<Type::Id>();
        const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
        switch (t) {
        case Type::DYNAMIC:
        case Type::STATIC:
            return std::make_shared<Model>(id, f, e, call);
        case Type::WIDGET:
            GXUNIMPLEMENTED
        }
    });
    c.set_data(m);
    return m;
}
