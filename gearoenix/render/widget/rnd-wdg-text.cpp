#include "rnd-wdg-text.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/stream/sys-stm-memory.hpp"
#include "../../system/sys-app.hpp"
#include "../font/rnd-fnt-2d.hpp"
#include "../font/rnd-fnt-manager.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-mesh.hpp"
#include "../shader/rnd-shd-shader.hpp"
#include <codecvt>
#include <locale>

void gearoenix::render::widget::Text::private_set_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    text = t;
    core::Real asp = 0.0f;
    core::Real starting_asp = 0.0f;
    std::uint8_t txtclr[4];
    txtclr[0] = (text_color[0] >= 1.0f ? 255 : static_cast<std::uint8_t>(text_color[0] * 255));
    txtclr[1] = (text_color[1] >= 1.0f ? 255 : static_cast<std::uint8_t>(text_color[1] * 255));
    txtclr[2] = (text_color[2] >= 1.0f ? 255 : static_cast<std::uint8_t>(text_color[2] * 255));
    txtclr[3] = 255;
    auto txtend = core::sync::EndCaller<texture::Texture2D>([c, this](const std::shared_ptr<texture::Texture2D>& txt) {
        meshes[text_mesh_id]->get_material()->set_color(txt);
    });
    auto txt = text_font->bake(text, txtclr, 0.02f, 5, asp, starting_asp, txtend);
    transformation->local_x_scale(asp / current_x_scale);
    current_x_scale = asp;
}

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
    GXTODO // Implement it in blender plugin too
        f->read(v_align);
    f->read(h_align);
    core::sync::EndCaller<font::Font> fend([c](const std::shared_ptr<font::Font>&) {});
    text_font = std::dynamic_pointer_cast<font::Font2D>(astmgr->get_font_manager()->get(f->read<core::Id>(), fend));
    auto mat = std::make_shared<material::Material>(f, e, c);
    mat->set_metallic_factor(0.001f);
    mat->set_roughness_factor(0.999f);
    mat->set_translucency(material::TranslucencyMode::Transparent);
    core::sync::EndCaller<mesh::Mesh> mend([c](const std::shared_ptr<mesh::Mesh>&) {});
    auto msh = astmgr->get_mesh_manager()->create_plate(mend);
    add_mesh(std::make_shared<model::Mesh>(msh, mat));
    private_set_text(text, c);
}

gearoenix::render::widget::Text::Text(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Text, e, c)
    , text(L" ")
    , text_color(0.999f, 0.999f, 0.999f)
{
    auto astmgr = e->get_system_application()->get_asset_manager();
    core::sync::EndCaller<font::Font> fend([c](const std::shared_ptr<font::Font>&) {});
    text_font = astmgr->get_font_manager()->get_default_2d(fend);
    auto mat = std::make_shared<material::Material>(e, c);
    mat->set_metallic_factor(0.001f);
    mat->set_roughness_factor(0.999f);
    mat->set_translucency(material::TranslucencyMode::Transparent);
    core::sync::EndCaller<mesh::Mesh> mend([c](const std::shared_ptr<mesh::Mesh>&) {});
    auto msh = astmgr->get_mesh_manager()->create_plate(mend);
    text_mesh_id = msh->get_asset_id();
    add_mesh(std::make_shared<model::Mesh>(msh, mat));
    private_set_text(text, c);
}

gearoenix::render::widget::Text::~Text() noexcept = default;

void gearoenix::render::widget::Text::set_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    private_set_text(t, c);
}

void gearoenix::render::widget::Text::set_text_color(
    const core::Real red,
    const core::Real green,
    const core::Real blue,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    text_color = math::Vec3(red, green, blue);
    set_text(text, c);
}
