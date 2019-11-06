#include "rnd-mdl-manager.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../widget/rnd-wdg-widget.hpp"
#include "rnd-mdl-dynamic.hpp"
#include "rnd-mdl-model.hpp"
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
        id, [id, c, this]() noexcept -> std::shared_ptr<Model> {
            system::stream::Stream* const f = cache.get_file();
            const auto t = f->read<Type>();
            const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
            switch (t) {
            case Type::Dynamic:
                return std::make_shared<Dynamic>(id, f, e, call);
            case Type::Static:
                return std::make_shared<Static>(id, f, e, call);
            case Type::Widget:
                return widget::Widget::read_gx3d(id, f, e, call);
            }
            GXLOGF("Unexpected type: " << static_cast<core::TypeId>(t) << ", in model: " << id)
        });
    c.set_data(m);
    return m;
}