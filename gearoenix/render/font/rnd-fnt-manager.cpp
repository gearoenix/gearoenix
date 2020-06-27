#include "rnd-fnt-manager.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/stream/sys-stm-asset.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-fnt-2d.hpp"

gearoenix::render::font::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

std::shared_ptr<gearoenix::render::font::Font> gearoenix::render::font::Manager::get(const core::Id id, core::sync::EndCaller<Font> c) noexcept
{
    auto f = cache.get<Font>(
        id, [id, c, this](std::string name) noexcept -> std::shared_ptr<Font> {
            system::stream::Stream* f = cache.get_file();
            switch (f->read<Type::Id>()) {
            case Type::D2:
                return std::make_shared<Font2D>(id, std::move(name), f, e->get_system_application()->get_asset_manager()->get_texture_manager());
            default:
                GXLOGF("Unexpected font type in: " << id)
            }
        });
    c.set_data(f);
    return f;
}

std::shared_ptr<gearoenix::render::font::Font2D> gearoenix::render::font::Manager::get_default_2d(core::sync::EndCaller<Font> c) noexcept
{
    if (default_2d == nullptr) {
        const auto& ast_mgr = e->get_system_application()->get_asset_manager();
        default_2d = std::make_shared<Font2D>(core::asset::Manager::create_id(), "default-2d", ast_mgr->get_texture_manager());
    }
    c.set_data(default_2d);
    return default_2d;
}
