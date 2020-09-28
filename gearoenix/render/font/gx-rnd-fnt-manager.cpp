#include "gx-rnd-fnt-manager.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../system/gx-sys-application.hpp"
#include "../../system/stream/gx-sys-stm-asset.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-fnt-2d.hpp"

gearoenix::render::font::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : cache(std::move(s))
    , e(e)
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
        default_2d = Font2D::construct_default(core::asset::Manager::create_id(), "default-2d", ast_mgr->get_texture_manager());
        if (default_2d == nullptr) {
            core::sync::EndCaller<Font> cc([c](const std::shared_ptr<Font>&) {});
            for (const auto& id_off : cache.get_offsets()) {
                default_2d = std::dynamic_pointer_cast<Font2D>(get(id_off.first, cc));
                if (nullptr != default_2d)
                    break;
            }
        }
    }
    c.set_data(default_2d);
    return default_2d;
}
