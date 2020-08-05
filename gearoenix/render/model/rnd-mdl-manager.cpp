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
        id, [id, c, this](std::string name) noexcept -> std::shared_ptr<Model> {
            GXLOGD("Going to load Model: " << id)
            system::stream::Stream* const f = cache.get_file();
            const auto t = f->read<Type>();
            const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
            switch (t) {
            case Type::Dynamic:
                return Dynamic::construct(id, std::move(name), f, e, call);
            case Type::Static:
                return Static::construct(id, std::move(name), f, e, call);
            default:
                return widget::Widget::read_gx3d(id, std::move(name), f, e, call);
            }
        });
    c.set_data(m);
    return m;
}

std::shared_ptr<gearoenix::render::model::Model> gearoenix::render::model::Manager::get_gx3d(
    const std::string& name,
    core::sync::EndCaller<Model>& c) noexcept
{
    return get_gx3d(cache.get_cacher().get_key(name), c);
}