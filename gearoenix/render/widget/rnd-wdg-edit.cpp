#include "rnd-wdg-edit.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../font/rnd-fnt-2d.hpp"
#include "../font/rnd-fnt-manager.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-dynamic.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../model/rnd-mdl-mesh.hpp"

gearoenix::render::widget::Edit::Edit(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Edit, f, e, c) {
        GXUNIMPLEMENTED
    }

    gearoenix::render::widget::Edit::Edit(const core::Id my_id, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Edit, e, c)
    , text_color(1.0f)
    , text_material(new material::Material(e, c))
    , hint_text_color(0.3f, 0.3f, 0.3f, 1.0f)
    , hint_text_material(new material::Material(e, c))
    , background_material(new material::Material(e, c))
    , cursor_material(new material::Material(e, c))
{
    auto* const astmgr = e->get_system_application()->get_asset_manager();
    auto* const mshmgr = astmgr->get_mesh_manager();
    auto* const mdlmgr = astmgr->get_model_manager();
    auto* const fntmgr = astmgr->get_font_manager();

    core::sync::EndCaller<mesh::Mesh> mshend([c](const std::shared_ptr<mesh::Mesh>&) {});
    const auto plate_mesh = mshmgr->create_plate(mshend);

    core::sync::EndCaller<font::Font> fend([c](const std::shared_ptr<font::Font>&) {});
    text_font = fntmgr->get_default_2d(fend);

    text_material->set_translucency(material::TranslucencyMode::Transparent);
    hint_text_material->set_translucency(material::TranslucencyMode::Transparent);

    core::sync::EndCaller<model::Dynamic> mdlend([c](const std::shared_ptr<model::Dynamic>&) {});

    text_model = mdlmgr->create<model::Dynamic>(mdlend);
    text_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, text_material));
    add_child(text_model);

    hint_text_model = mdlmgr->create<model::Dynamic>(mdlend);
    hint_text_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, hint_text_material));
    hint_text_model->get_transformation()->set_location(math::Vec3(0.0f, 0.0f, 0.01f));
    add_child(hint_text_model);

    background_material->set_color(0.02f, 0.02f, 0.02f, c);
    background = mdlmgr->create<model::Dynamic>(mdlend);
    background->add_mesh(std::make_shared<model::Mesh>(plate_mesh, background_material));
    add_child(background);

    cursor_material->set_color(0.0f, 0.0f, 0.0f, c);
    cursor = mdlmgr->create<model::Dynamic>(mdlend);
    cursor->add_mesh(std::make_shared<model::Mesh>(plate_mesh, cursor_material));
    cursor->get_transformation()->set_location(math::Vec3(0.0f, 0.0f, -0.01f));
    add_child(cursor);

    /// TODO: It can register itself in on scale of transformation
}

gearoenix::render::widget::Edit::~Edit() noexcept = default;

void gearoenix::render::widget::Edit::set_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::render::widget::Edit::set_hint_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    hint_text = t;
    const auto& scale = collider->get_scale();
    const auto img_height = scale[1] * 2.0f;
    text_font->compute_text_widths(t, img_height, hint_text_widths);
    const auto raw_img_width = scale[0] * 2.0f;
    const auto is_bigger = raw_img_width < hint_text_widths.back();
    const auto img_width = is_bigger ? raw_img_width : hint_text_widths.back();
    core::sync::EndCaller<texture::Texture2D> txtend([c, this](const std::shared_ptr<texture::Texture2D>& txt) {
        hint_text_material->set_color(txt);
    });
    const auto _txt = text_font->bake(
        hint_text, hint_text_widths, hint_text_color,
        img_width, img_height, 0.0f, txtend);
    hint_text_model->get_transformation()->local_x_scale(
        img_width / (hint_text_model->get_collider()->get_scale()[0] * 2.0f));
}