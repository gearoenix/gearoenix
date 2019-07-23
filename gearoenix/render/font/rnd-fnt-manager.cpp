#include "rnd-fnt-manager.hpp"
#include "rnd-fnt-2d.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"

gearoenix::render::font::Manager::Manager(system::stream::Stream* const s, engine::Engine* const e) noexcept
    : e(e)
    , cache(s)
{
}

std::shared_ptr<gearoenix::render::font::Font> gearoenix::render::font::Manager::get(const core::Id id, core::sync::EndCaller<Font> c) noexcept
{
    const auto f = cache.get<Font>(id, [id, c, this]() noexcept -> std::shared_ptr<Font> {
        system::stream::Stream* f = cache.get_file();
        const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
        switch (f->read<Type::Id>()) {
        case Type::D2:
            return std::make_shared<Font2D>(id, f, e->get_system_application()->get_asset_manager()->get_texture_manager());
        default:
            GXLOGF("Unexpected font type in: " << id)
        }
    });
    c.set_data(f);
    return f;
}
