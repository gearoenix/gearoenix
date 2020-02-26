#include "rnd-wdg-text.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../physics/collider/phs-cld-collider.hpp"
#include "../../system/stream/sys-stm-memory.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../font/rnd-fnt-2d.hpp"
#include "../font/rnd-fnt-manager.hpp"
#include "../material/rnd-mat-unlit.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-mesh.hpp"
#include "../shader/rnd-shd-shader.hpp"
#include <codecvt>
#include <locale>

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
    auto ast_mgr = e->get_system_application()->get_asset_manager();
    GXTODO // Implement it in blender plugin too
        f->read(v_align);
    f->read(h_align);
    core::sync::EndCaller<font::Font> fend([c](const std::shared_ptr<font::Font>&) {});
    text_font = std::dynamic_pointer_cast<font::Font2D>(ast_mgr->get_font_manager()->get(f->read<core::Id>(), fend));
    auto mat = std::make_shared<material::Unlit>(f, e, c);
    mat->set_translucency(material::TranslucencyMode::Transparent);
    core::sync::EndCaller<mesh::Mesh> mend([c](const std::shared_ptr<mesh::Mesh>&) {});
    auto msh = ast_mgr->get_mesh_manager()->create_plate(mend);
    add_mesh(std::make_shared<model::Mesh>(msh, mat));
    set_text(text, c);
}

gearoenix::render::widget::Text::Text(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Text, e, c)
    , text(L" ")
    , text_color(0.7f, 0.7f, 0.7f, 1.0f)
{
    auto ast_mgr = e->get_system_application()->get_asset_manager();
    core::sync::EndCaller<font::Font> fend([c](const std::shared_ptr<font::Font>&) {});
    text_font = ast_mgr->get_font_manager()->get_default_2d(fend);
    auto mat = std::make_shared<material::Unlit>(e, c);
    mat->set_translucency(material::TranslucencyMode::Transparent);
    core::sync::EndCaller<mesh::Mesh> mend([c](const std::shared_ptr<mesh::Mesh>&) {});
    auto msh = ast_mgr->get_mesh_manager()->create_plate(mend);
    text_mesh_id = msh->get_asset_id();
    add_mesh(std::make_shared<model::Mesh>(msh, mat));
    set_text(text, c);
}

gearoenix::render::widget::Text::~Text() noexcept = default;

void gearoenix::render::widget::Text::set_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    text = t;
    core::Real img_width = 0.0f;
    auto txt_end = core::sync::EndCaller<texture::Texture2D>([c, this](const std::shared_ptr<texture::Texture2D>& txt) {
        reinterpret_cast<material::Unlit*>(meshes[text_mesh_id]->get_mat().get())->set_color(txt);
    });
    auto txt = text_font->bake(text, text_color, collider->get_current_local_scale()[1] * 2.0f, img_width, txt_end);
    transformation->local_x_scale(img_width * 0.5f / collider->get_current_local_scale()[0]);
}

void gearoenix::render::widget::Text::set_text_color(
    const core::Real red,
    const core::Real green,
    const core::Real blue,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    set_text_color(math::Vec4(red, green, blue, 1.0f), c);
}

void gearoenix::render::widget::Text::set_text_color(
    const math::Vec4& v, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    text_color = v;
    set_text(text, c);
}
