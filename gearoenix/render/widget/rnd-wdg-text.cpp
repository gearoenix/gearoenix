#include "rnd-wdg-text.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/stream/sys-stm-memory.hpp"
#include "../../system/sys-app.hpp"
#include "../font/rnd-fnt-2d.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../rnd-engine.hpp"

std::pair<gearoenix::core::Id, std::shared_ptr<gearoenix::render::mesh::Mesh>> gearoenix::render::widget::Text::create_text_mesh(core::EndCaller<core::EndCallerIgnore> c)
{
    system::stream::Memory ms;
    system::stream::Stream& s = ms;
    s.write(mesh::Mesh::Geo::BASIC);
    s.write<core::Count>(5);
    s.write<core::Count>(4 * text.size());
    for (const char c : text) {
        const font::Font2D::LetterProperties& lp = fnt->get_letter_properties(c);

        s.write(lp.pos_min);
        s.write<core::Real>(0.0f);
        s.write(lp.uv_min);

        s.write(lp.pos_min[0]);
        s.write(lp.pos_max[1]);
        s.write<core::Real>(0.0f);
        s.write(lp.uv_min[0]);
        s.write(lp.uv_max[1]);

        s.write(lp.pos_min[1]);
        s.write(lp.pos_max[0]);
        s.write<core::Real>(0.0f);
        s.write(lp.uv_min[1]);
        s.write(lp.uv_max[0]);

        s.write(lp.pos_max);
        s.write<core::Real>(0.0f);
        s.write(lp.uv_max);
    }
    s.write<core::Count>(6 * text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        std::uint32_t index = (std::uint32_t)i << 2;
        s.write(index);
        s.write(index + 1);
        s.write(index + 2);
        s.write(index + 1);
        s.write(index + 2);
        s.write(index + 3);
    }
    s.seek(0);
    core::Id mesh_id = render_engine->get_system_application()->get_asset_manager()->create_id();
    //    return std::make_pair(mesh_id, std::shared_ptr<mesh::Mesh>(mesh::Mesh::read(&s, render_engine, c)));
    return std::make_pair(0, nullptr);
}

gearoenix::render::widget::Text::Text(system::stream::Stream* s, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Widget(s, e, c)
    , text(s->read_string())
    , align(s->read<Alignment::Type>())
{
    fnt = std::static_pointer_cast<font::Font2D>(
        e->get_system_application()->get_asset_manager()->get_font(
            s->read<core::Id>(), core::EndCaller<font::Font>(
                                     [c](std::shared_ptr<font::Font>) -> void {
                                     })));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::pair<core::Id, std::shared_ptr<mesh::Mesh>> id_meshdddd = create_text_mesh(c);
}

gearoenix::render::widget::Text::~Text() {}
