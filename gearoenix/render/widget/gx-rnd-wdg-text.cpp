#include "gx-rnd-wdg-text.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../font/gx-rnd-fnt-font.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../model/gx-rnd-mdl-builder.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"

gearoenix::render::widget::Text::Text(
    std::string&& n,
    engine::Engine& e)
    : Widget(std::move(n), Type::Text, e)
{
}

void gearoenix::render::widget::Text::construct(
    std::string&& name,
    const core::ecs::entity_id_t camera_id,
    std::shared_ptr<material::Unlit>&& mat,
    std::shared_ptr<Widget>&& parent,
    std::shared_ptr<scene::Builder>&& scene_builder,
    core::job::EndCaller<std::pair<std::shared_ptr<Text>, std::shared_ptr<model::Builder>>>&& end_callback)
{
    auto& e = scene_builder->e;
    e.get_mesh_manager()->build_plate(std::move(mat), core::job::EndCallerShared<mesh::Mesh>([n = std::move(name), c = camera_id, p = std::move(parent), s = std::move(scene_builder), e = std::move(end_callback)](std::shared_ptr<mesh::Mesh>&& msh) mutable {
        construct(std::move(n), c, std::move(msh), std::move(p), std::move(s), std::move(e));
    }));
}

void gearoenix::render::widget::Text::construct(
    std::string&& name,
    const core::ecs::entity_id_t camera_id,
    std::shared_ptr<Widget>&& parent,
    std::shared_ptr<scene::Builder>&& scene_builder,
    core::job::EndCaller<std::pair<std::shared_ptr<Text>, std::shared_ptr<model::Builder>>>&& end_callback)
{
    auto& e = scene_builder->e;
    const auto mat_name = name + "-material";
    e.get_material_manager()->get_unlit(mat_name, core::job::EndCallerShared<material::Unlit>([n = std::move(name), c = camera_id, p = std::move(parent), s = std::move(scene_builder), e = std::move(end_callback)](std::shared_ptr<material::Unlit>&& mat) mutable {
        construct(std::move(n), c, std::move(mat), std::move(p), std::move(s), std::move(e));
    }));
}

void gearoenix::render::widget::Text::construct(
    std::string&& name,
    const core::ecs::entity_id_t camera_id,
    std::shared_ptr<mesh::Mesh>&& msh,
    std::shared_ptr<Widget>&& parent,
    std::shared_ptr<scene::Builder>&& scene_builder,
    core::job::EndCaller<std::pair<std::shared_ptr<Text>, std::shared_ptr<model::Builder>>>&& end_callback)
{
    auto& e = scene_builder->e;
    std::shared_ptr<Text> r(new Text(std::move(name), e));
    r->text_material = std::dynamic_pointer_cast<material::Unlit>(msh->get_bound_material());
    r->text_material->set_transparency(material::Transparency::Transparent);
    r->text_self = r;
    auto model_builder = e.get_model_manager()->build(
        r->name + "-model-text",
        { std::move(msh) },
        core::job::EndCaller([] {}),
        true);
    r->set_model_entity_id(model_builder->get_id());
    r->set_camera_entity_id(camera_id);
    scene_builder->add(std::shared_ptr(model_builder));
    if (parent != nullptr) {
        parent->add_child(r);
    }
    r->parent = std::move(parent);
    end_callback.set_return({ std::move(r), std::move(model_builder) });
}

gearoenix::render::widget::Text::~Text()
{
    e.get_world()->delayed_delete_entity(model_entity_id, core::job::EndCaller([] {}));
}

gearoenix::math::Vec2<double> gearoenix::render::widget::Text::get_text_size() const
{
    auto* const trn = e.get_world()->get_component<physics::TransformationComponent>(model_entity_id);
    GX_ASSERT_D(nullptr != trn);
    return trn->get_scale().xy() * 2.0; // TODO not sure about 2 multiplication
}

void gearoenix::render::widget::Text::update_text(const core::job::EndCaller<>& c) const
{
    const auto img_dim = get_text_size();
    double width = 0.0;
    text_font->bake(
        text, text_colour, img_dim.y, width,
        core::job::EndCallerShared<texture::Texture2D>([c, self = text_self.lock()](std::shared_ptr<texture::Texture2D>&& t) {
            self->text_material->set_albedo(std::move(t));
        }));
    e.get_world()->get_component<physics::TransformationComponent>(model_entity_id)->local_x_scale(width / img_dim.x);
}
