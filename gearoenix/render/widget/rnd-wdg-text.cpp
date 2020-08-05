#include "rnd-wdg-text.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../physics/collider/phs-cld-collider.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../font/rnd-fnt-2d.hpp"
#include "../font/rnd-fnt-manager.hpp"
#include "../material/rnd-mat-unlit.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-dynamic.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../model/rnd-mdl-mesh.hpp"
#include "../shader/rnd-shd-shader.hpp"
#include <codecvt>
#include <locale>

void gearoenix::render::widget::Text::initialize(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    auto ast_mgr = e->get_system_application()->get_asset_manager();
    text_mesh_material = std::make_shared<material::Unlit>(e, c);
    text_mesh_material->set_translucency(material::TranslucencyMode::Transparent);
    core::sync::EndCaller<mesh::Mesh> mend([c](const std::shared_ptr<mesh::Mesh>&) {});
    core::sync::EndCaller<model::Dynamic> dyn_mdl_end([c](const std::shared_ptr<model::Dynamic>&) {});
    auto msh = ast_mgr->get_mesh_manager()->create_plate(mend);
    text_model = ast_mgr->get_model_manager()->create<model::Dynamic>(
        "text-" + name + "-model", dyn_mdl_end);
    text_model->add_mesh(std::make_shared<model::Mesh>(msh, text_mesh_material));
    text_model->get_transformation()->local_scale(collider->get_current_local_scale());
    add_child(text_model);
    set_text(text, c);
}

void gearoenix::render::widget::Text::update_alignment() noexcept
{
    math::Vec3 trl(0.0);
    switch (h_align) {
    case Alignment::Center:
        break;
    case Alignment::Start:
        trl.x = get_text_width() * 0.5;
        break;
    case Alignment::End:
        trl.x = get_text_width() * -0.5;
        break;
    default:
        break;
    }
    switch (v_align) {
    case Alignment::Center:
        break;
    case Alignment::Start:
        trl.y = get_text_height() * -0.5;
        break;
    case Alignment::End:
        trl.y = get_text_height() * 0.5;
        break;
    default:
        break;
    }
    text_model->get_transformation()->set_location(transformation->get_location() + trl);
}

gearoenix::render::widget::Text::Text(
    const core::Id my_id,
    std::string name,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, std::move(name), Type::Text, f, e, c)
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
    f->read(v_align);
    f->read(h_align);
    core::sync::EndCaller<font::Font> fend([c](const std::shared_ptr<font::Font>&) {});
    const auto font_id = f->read<core::Id>();
    math::Vec4<float> color;
    color.read(f);
    text_color = math::Vec4<double>(color);
    text_font = std::dynamic_pointer_cast<font::Font2D>(ast_mgr->get_font_manager()->get(font_id, fend));
    initialize(c);
}

gearoenix::render::widget::Text::Text(
    const core::Id my_id,
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, std::move(name), Type::Text, e, c)
    , text_color(0.7, 0.7, 0.7, 1.0)
{
    auto ast_mgr = e->get_system_application()->get_asset_manager();
    core::sync::EndCaller<font::Font> fend([c](const std::shared_ptr<font::Font>&) {});
    text_font = ast_mgr->get_font_manager()->get_default_2d(fend);
    initialize(c);
}

#define GX_TEST_CONS(...)                              \
    std::shared_ptr<Text> self(new Text(__VA_ARGS__)); \
    self->model_self = self;                           \
    self->widget_self = self;                          \
    self->text_self = self;                            \
    return self

std::shared_ptr<gearoenix::render::widget::Text> gearoenix::render::widget::Text::construct(
    const core::Id id,
    std::string name,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    GX_TEST_CONS(id, std::move(name), f, e, c);
}

std::shared_ptr<gearoenix::render::widget::Text> gearoenix::render::widget::Text::construct(
    const core::Id id,
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    GX_TEST_CONS(id, std::move(name), e, c);
}

gearoenix::render::widget::Text::~Text() noexcept = default;

void gearoenix::render::widget::Text::set_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    text = t;
    double img_width = 0.0;
    auto txt_end = core::sync::EndCaller<texture::Texture2D>([c, t, this](const std::shared_ptr<texture::Texture2D>& txt) {
        // This is because of controlling latency and over-assignments
        if (t != text)
            return;
        text_mesh_material->set_color(txt);
    });
    auto txt = text_font->bake(text, text_color, get_text_height(), img_width, txt_end);
    text_model->get_transformation()->local_x_scale(img_width / get_text_width());
    update_alignment();
}

void gearoenix::render::widget::Text::set_text_color(
    const double red,
    const double green,
    const double blue,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    set_text_color(math::Vec4(red, green, blue, 1.0), c);
}

void gearoenix::render::widget::Text::set_text_color(
    const math::Vec4<double>& v, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    text_color = v;
    set_text(text, c);
}

double gearoenix::render::widget::Text::get_text_width() const noexcept
{
    return text_model->get_collider()->get_current_local_scale().x * 2.0;
}
double gearoenix::render::widget::Text::get_text_height() const noexcept
{
    return text_model->get_collider()->get_current_local_scale().y * 2.0;
}
