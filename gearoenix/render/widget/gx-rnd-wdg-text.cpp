#include "gx-rnd-wdg-text.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../font/gx-rnd-fnt-font.hpp"
#include "../font/gx-rnd-fnt-manager.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../model/gx-rnd-mdl-builder.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"

gearoenix::render::widget::Text::Text(
    std::string&& n,
    engine::Engine& e,
    scene::Builder& scene_builder,
    const core::sync::EndCaller& c) noexcept
    : Widget(std::move(n), Type::Text, e)
{
    text_material = e.get_material_manager()->get_unlit(name, c);
    text_material->set_transparency(material::Transparency::Transparent);
    auto model_builder = e.get_model_manager()->build(
        "gearoenix-splash-text",
        e.get_mesh_manager()->build_plate(c),
        std::shared_ptr(text_material),
        core::sync::EndCaller(c),
        true);
    text_model = model_builder->get_id();
    scene_builder.add(std::move(model_builder));
}

std::shared_ptr<gearoenix::render::widget::Text> gearoenix::render::widget::Text::construct(
    std::string&& name,
    engine::Engine& e,
    scene::Builder& scene_builder,
    const core::sync::EndCaller& c) noexcept
{
    std::shared_ptr<Text> r(new Text(std::move(name), e, scene_builder, c));
    r->text_self = r;
    return r;
}

gearoenix::render::widget::Text::~Text() noexcept
{
    e.get_world()->delayed_remove_entity(text_model);
}

gearoenix::math::Vec2<double> gearoenix::render::widget::Text::get_text_size() const noexcept
{
    return { 0.0, 0.0 };
}

void gearoenix::render::widget::Text::update_text(const core::sync::EndCaller&) noexcept
{
}