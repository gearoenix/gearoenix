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
#include "../mesh/rnd-msh-manager.hpp"
#include "../model/rnd-mdl-mesh.hpp"
#include "../model/rnd-mdl-transformation.hpp"
#include <locale>
#include <codecvt>

gearoenix::render::widget::Text::Text(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Text, f, e, c)
{
    {
        // Reading string
        const auto s = f->read_string();
        // I know it is deprecated but because it does not have any equivalent in std
        // I'm going to keep it, till new replacement in std has been implemented.
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        text = converter.from_bytes(s);
    }
    auto astmgr = e->get_system_application()->get_asset_manager();
    f->read(align);
    core::sync::EndCaller<font::Font> fend([c](std::shared_ptr<font::Font>) {});
    fnt = std::dynamic_pointer_cast<font::Font2D>(astmgr->get_font_manager()->get(f->read<core::Id>(), fend));
    auto mat = std::make_shared<material::Material>(f, e, c);
    core::Real asp = 0.0f;
    auto txt = fnt->multiline_bake(text, text_color, 512, 512, 5, asp, c);
    mat->set_color(txt);
    mat->set_metallic_factor(0.001);
    mat->set_roughness_factor(0.999);
    core::sync::EndCaller<mesh::Mesh> mend([c](std::shared_ptr<mesh::Mesh>) {});
    auto msh = astmgr->get_mesh_manager()->create_plate(mend);
    add_mesh(std::make_shared<model::Mesh>(msh, mat));
    if (asp > 1.0f)
        transformation->local_x_scale(asp * 2.0f);
    else
        GXUNIMPLEMENTED
}

gearoenix::render::widget::Text::~Text() noexcept {}
