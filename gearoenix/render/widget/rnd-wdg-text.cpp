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
    text_font = std::dynamic_pointer_cast<font::Font2D>(astmgr->get_font_manager()->get(f->read<core::Id>(), fend));
    auto mat = std::make_shared<material::Material>(f, e, c);
    mat->set_metallic_factor(0.001);
    mat->set_roughness_factor(0.999);
    core::sync::EndCaller<mesh::Mesh> mend([c](std::shared_ptr<mesh::Mesh>) {});
    auto msh = astmgr->get_mesh_manager()->create_plate(mend);
    add_mesh(std::make_shared<model::Mesh>(msh, mat));
    set_text(text);
}

gearoenix::render::widget::Text::Text(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Text, e, c)
    , text(L" ")
{
    auto astmgr = e->get_system_application()->get_asset_manager();
    core::sync::EndCaller<font::Font> fend([c](std::shared_ptr<font::Font>) {});
    text_font = astmgr->get_font_manager()->get_default_2d(fend);
    auto mat = std::make_shared<material::Material>(e, c);
    mat->set_metallic_factor(0.001);
    mat->set_roughness_factor(0.999);
    core::sync::EndCaller<mesh::Mesh> mend([c](std::shared_ptr<mesh::Mesh>) {});
    auto msh = astmgr->get_mesh_manager()->create_plate(mend);
    text_mesh_id = msh->get_asset_id();
    add_mesh(std::make_shared<model::Mesh>(msh, mat));
    set_text(text);
}

gearoenix::render::widget::Text::~Text() noexcept {}

void gearoenix::render::widget::Text::set_text(const std::wstring& t) noexcept
{
    text = t;
    auto &mat = meshes[text_mesh_id]->get_material();
    core::Real asp = 0.0f;
    std::uint32_t txtclr = 0;
    txtclr |= text_color[0] >= 1.0f ? 255 << 24 : static_cast<std::uint32_t>(text_color[0] * 255) << 24;
    txtclr |= text_color[1] >= 1.0f ? 255 << 16 : static_cast<std::uint32_t>(text_color[1] * 255) << 16;
    txtclr |= text_color[2] >= 1.0f ? 255 << 8 : static_cast<std::uint32_t>(text_color[2] * 255) << 8;
    txtclr |= 255;
    auto txt = text_font->multiline_bake(text, txtclr, 512, 512, 5, asp);
    mat->set_color(txt);
    current_x_scale = 2.0f * asp;
    transformation->local_x_scale(asp * 2.0f);
}
