#include "rnd-wdg-text.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/stream/sys-stm-memory.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../font/rnd-fnt-2d.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../shader/rnd-shd-shader.hpp"

void gearoenix::render::widget::Text::create_text_mesh(core::sync::EndCaller<core::sync::EndCallerIgnore> c) noexcept
{
    system::stream::Memory ms;
    system::stream::Stream& s = ms;
    const core::Count text_size = (core::Count)text.size();
    s.write(mesh::Type::BASIC);
    s.write<core::Count>(5);
    s.write(text_size * 4);
    struct Vertex {
        math::Vec3 pos = math::Vec3(0.0f);
        math::Vec2 uv = math::Vec2(0.0f);
    };
    struct CharacterVerices {
        Vertex v[4];
        core::Real w = 0.0f, h = 0.0f;
    };
    std::vector<CharacterVerices> cvs(text_size);
    std::map<char, font::Font2D::LetterProperties> lps;
    core::Real textw = 0.0f;
    for (const char c : text) {
        const font::Font2D::LetterProperties& lp = fnt->get_letter_properties(c);
        lps[c] = lp;
        textw += lp.pos_max[0] - lp.pos_min[0] + space_character;
        if (c == ' ')
            textw += space_word;
    }
    core::Real starting_x;
    switch (align) {
    case Alignment::CENTER_BOTTOM:
    case Alignment::CENTER_MIDDLE:
    case Alignment::CENTER_TOP:
        starting_x = textw * -0.5f;
        break;
    case Alignment::LEFT_BOTTOM:
    case Alignment::LEFT_MIDDLE:
    case Alignment::LEFT_TOP:
        starting_x = 0.0f;
        break;
    case Alignment::RIGHT_BOTTOM:
    case Alignment::RIGHT_MIDDLE:
    case Alignment::RIGHT_TOP:
        starting_x = -textw;
        break;
    default:
        GXUNEXPECTED;
    }
    core::Real starting_y;
    switch (align) {
    case Alignment::CENTER_BOTTOM:
    case Alignment::LEFT_BOTTOM:
    case Alignment::RIGHT_BOTTOM:
        starting_y = 1.0f;
        break;
    case Alignment::CENTER_MIDDLE:
    case Alignment::LEFT_MIDDLE:
    case Alignment::RIGHT_MIDDLE:
        starting_y = 0.5f;
        break;
    case Alignment::CENTER_TOP:
    case Alignment::LEFT_TOP:
    case Alignment::RIGHT_TOP:
        starting_y = 0.0f;
        break;
    default:
        GXUNEXPECTED;
    }
    for (core::Count i = 0; i < text_size; ++i) {
        const char c = text[i];
        const font::Font2D::LetterProperties& lp = lps[c];
        const math::Vec3 pos_min(lp.pos_min[0] + starting_x, lp.pos_min[1] + starting_y, 0.0f);
        const math::Vec3 pos_max(lp.pos_max[0] + starting_x, lp.pos_max[1] + starting_y, 0.0f);
        starting_x += lp.pos_max[0] - lp.pos_min[0] + space_character;
        if (c == ' ')
            starting_x += space_word;

        cvs[i].v[0].pos = pos_min;
        cvs[i].v[0].uv = math::Vec2(lp.uv_min[0], lp.uv_max[1]);

        cvs[i].v[1].pos = math::Vec3(pos_max[0], pos_min[1], 0.0f);
        cvs[i].v[1].uv = lp.uv_max;

        cvs[i].v[2].pos = pos_max;
        cvs[i].v[2].uv = math::Vec2(lp.uv_max[0], lp.uv_min[1]);

        cvs[i].v[3].pos = math::Vec3(pos_min[0], pos_max[1], 0.0f);
        cvs[i].v[3].uv = lp.uv_min;
    }
    for (core::Count i = 0; i < text_size; ++i) {
        s.write(cvs[i].v);
    }
    s.write(text_size * 6);
    for (core::Real i = 0; i < text_size; ++i) {
        std::uint32_t index = (std::uint32_t)i << 2;
        s.write(index);
        s.write(index + 1);
        s.write(index + 2);
        s.write(index);
        s.write(index + 2);
        s.write(index + 3);
    }
    s.seek(0);
    mesh_id = e->get_system_application()->get_asset_manager()->create_id();
    //msh = std::shared_ptr<mesh::Mesh>(mesh::Mesh::read(mesh_id, s, e, c));
}

gearoenix::render::widget::Text::Text(
    const core::Id my_id,
    system::stream::Stream*const f,
    engine::Engine*const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, f, e, c)
    , text(f->read_string())
    , align(f->read<Alignment::Type>())
    , space_character(f->read<core::Real>())
    , space_word(f->read<core::Real>())
    , space_line(f->read<core::Real>())
{
    core::Id font_id;
    f->read(font_id);
    f->read<core::Id>();
    math::Vec4 color;
    //color.read(f);
    //fnt = std::static_pointer_cast<font::Font2D>(
    //    e->get_system_application()->get_asset_manager()->get_font_manager()->get(
    //        font_id, core::sync::EndCaller<font::Font>([c](std::shared_ptr<font::Font>) -> void {
    //        })));
    //create_text_mesh(c);
    //material::FontColored* font_mat = new material::FontColored(shader::FONT_COLORED, fnt->get_baked_texture(), e, c);
    //font_mat->set_color(color);
    //std::shared_ptr<material::Material> mat(font_mat);
    //std::shared_ptr<material::Depth> dp = nullptr;
    //if (shader::Shader::is_shadow_caster(mat->get_shader_id())) {
    //    dp = std::shared_ptr<material::Depth>(
    //        new material::Depth(
    //            shader::Shader::get_shadow_caster_shader_id(mat->get_shader_id()),
    //            e, c));
    //    has_shadow_caster = true;
    //}
    //has_transparent |= shader::Shader::is_transparent(mat->get_shader_id());
    //needs_mvp |= mat->needs_mvp();
    //needs_dbm |= mat->needs_dbm();
    //meshes[mesh_id] = std::make_tuple(msh, mat, dp);
}

gearoenix::render::widget::Text::~Text() noexcept {}
