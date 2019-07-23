#include "rnd-wdg-text.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/stream/sys-stm-memory.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../font/rnd-fnt-2d.hpp"
#include "../font/rnd-fnt-manager.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../shader/rnd-shd-shader.hpp"
#include <locale>
#include <codecvt>

gearoenix::render::widget::Text::Text(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, f, e, c)
{
    {
        // Reading string
        const auto s = f->read_string();
        // I know it is deprecated but because it does not have any equivalent in std
        // I'm going to keep it, till new replacement in std has been implemented.
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        text = converter.from_bytes(s);
    }
    f->read(align);
    fnt = std::dynamic_pointer_cast<font::Font2D>(
        e->get_system_application()->get_asset_manager()->get_font_manager()->get(
            f->read<core::Id>(),
            core::sync::EndCaller<font::Font>([c](std::shared_ptr<font::Font>) {})));
    auto mat = std::make_shared<material::Material>(f, e, c);
}

gearoenix::render::widget::Text::~Text() noexcept {}
