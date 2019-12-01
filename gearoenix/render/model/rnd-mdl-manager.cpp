#include "rnd-mdl-manager.hpp"
#include "../widget/rnd-wdg-widget.hpp"
#include "rnd-mdl-dynamic.hpp"
#include "rnd-mdl-static.hpp"
#include "rnd-mdl-type.hpp"

gearoenix::render::model::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

std::shared_ptr<gearoenix::render::model::Model> gearoenix::render::model::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Model>& c) noexcept
{
    std::shared_ptr<Model> m = cache.get<Model>(
        id, [id, c, this]() noexcept {
            GXLOGD("Going to load Model: " << id)
            system::stream::Stream* const f = cache.get_file();
            const auto t = f->read<Type>();
            const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
            switch (t) {
            case Type::Dynamic:
                return std::shared_ptr<Model>(new Dynamic(id, f, e, call));
            case Type::Static:
                return std::shared_ptr<Model>(new Static(id, f, e, call));
            default:
                return std::dynamic_pointer_cast<Model>(widget::Widget::read_gx3d(id, f, e, call));
            }
        });
    c.set_data(m);
    return m;
}