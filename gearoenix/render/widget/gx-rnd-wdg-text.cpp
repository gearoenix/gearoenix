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
#include "../model/gx-rnd-mdl-model.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"

gearoenix::render::widget::Text::Text(
    std::string&& n,
    engine::Engine& e,
    const core::sync::EndCaller& c) noexcept
    : Widget(std::move(n), Type::Text, e)
{
    text_material = e.get_material_manager()->get_unlit(name, c);
    text_material->set_transparency(material::Transparency::Transparent);
}

std::pair<std::shared_ptr<gearoenix::render::widget::Text>, std::shared_ptr<gearoenix::render::model::Builder>> gearoenix::render::widget::Text::construct(
    std::string&& name,
    engine::Engine& e,
    const core::ecs::entity_id_t camera_id,
    Widget* const parent,
    scene::Builder& scene_builder,
    const core::sync::EndCaller& c) noexcept
{
    std::shared_ptr<Text> r(new Text(std::move(name), e, c));
    r->text_self = r;
    auto model_builder = e.get_model_manager()->build(
        r->name + "-model-text",
        e.get_mesh_manager()->build_plate(c),
        std::shared_ptr(r->text_material),
        core::sync::EndCaller(c),
        true);
    r->set_model_entity_id(model_builder->get_id());
    r->set_camera_entity_id(camera_id);
    scene_builder.add(std::shared_ptr(model_builder));
    if (parent != nullptr) {
        parent->add_child(r);
    }
    return { std::move(r), std::move(model_builder) };
}

gearoenix::render::widget::Text::~Text() noexcept
{
    e.get_world()->delayed_remove_entity(model_entity_id);
}

gearoenix::math::Vec2<double> gearoenix::render::widget::Text::get_text_size() const noexcept
{
    auto* const trn = e.get_world()->get_component<physics::Transformation>(model_entity_id);
    GX_ASSERT_D(nullptr != trn);
    return trn->get_scale().xy() * 2.0; // TODO not sure about 2 multiplication
}

void gearoenix::render::widget::Text::update_text(const core::sync::EndCaller& c) noexcept
{
    const auto img_dim = get_text_size();
    double width = 0.0;
    const std::shared_ptr<std::shared_ptr<texture::Texture2D>> txt(new std::shared_ptr<texture::Texture2D>());
    *txt = text_font->bake(text, text_colour, img_dim.y, width, core::sync::EndCaller([txt, c, self = text_self.lock()] {
        self->text_material->set_albedo(*txt);
    }));
    e.get_world()->get_component<physics::Transformation>(model_entity_id)->local_x_scale(width / img_dim.x);
}