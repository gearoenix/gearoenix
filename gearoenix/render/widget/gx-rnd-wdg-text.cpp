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
#include "../model/gx-rnd-mdl-manager.hpp"

gearoenix::render::widget::Text::Text(std::string&& n)
    : Widget(std::move(n), Type::Text)
{
}

void gearoenix::render::widget::Text::construct(std::string&& name, core::ecs::Entity* const camera_entity, std::shared_ptr<material::Unlit>&& mat, std::shared_ptr<Widget>&& parent, core::job::EndCallerShared<Text>&& end_callback)
{
    mesh::Manager::get().build_plate(std::move(mat), core::job::EndCallerShared<mesh::Mesh>([n = std::move(name), c = camera_entity, p = std::move(parent), e = std::move(end_callback)](std::shared_ptr<mesh::Mesh>&& msh) mutable {
        construct(std::move(n), c, std::move(msh), std::move(p), std::move(e));
    }));
}

void gearoenix::render::widget::Text::construct(std::string&& name, core::ecs::Entity* const camera_entity, std::shared_ptr<Widget>&& parent, core::job::EndCallerShared<Text>&& end_callback)
{
    material::Manager::get().get_unlit(name + "-material", core::job::EndCallerShared<material::Unlit>([n = std::move(name), c = camera_entity, p = std::move(parent), e = std::move(end_callback)](std::shared_ptr<material::Unlit>&& mat) mutable {
        construct(std::move(n), c, std::move(mat), std::move(p), std::move(e));
    }));
}

void gearoenix::render::widget::Text::construct(std::string&& name, core::ecs::Entity* const camera_entity, std::shared_ptr<mesh::Mesh>&& msh, std::shared_ptr<Widget>&& parent, core::job::EndCallerShared<Text>&& end_callback)
{
    std::shared_ptr<Text> r(new Text(std::move(name)));
    r->text_material = std::dynamic_pointer_cast<material::Unlit>(msh->get_bound_material());
    r->text_material->set_transparency(material::Transparency::Transparent);
    r->text_self = r;
    auto model_entity = model::Manager::get().build(r->name + "-model-text", parent ? parent->get_model_entity().get() : nullptr, { std::move(msh) }, true);
    r->set_model_entity(std::move(model_entity));
    r->set_camera_entity(camera_entity);
    if (parent != nullptr) {
        parent->add_child(r);
    }
    r->parent = std::move(parent);
    end_callback.set_return(std::move(r));
}

gearoenix::render::widget::Text::~Text() { }

gearoenix::math::Vec2<double> gearoenix::render::widget::Text::get_text_size() const
{
    auto* const trn = model_entity->get_component<physics::Transformation>();
    GX_ASSERT_D(nullptr != trn);
    return trn->get_scale().xy() * 2.0; // TODO not sure about 2 multiplication
}

void gearoenix::render::widget::Text::update_text(const core::job::EndCaller<>& c) const
{
    const auto img_dim = get_text_size();
    double width = 0.0;
    text_font->bake(text, text_colour, img_dim.y, width, core::job::EndCallerShared<texture::Texture2D>([c, self = text_self.lock()](std::shared_ptr<texture::Texture2D>&& t) {
        self->text_material->set_albedo(std::move(t));
        (void)c;
    }));
    model_entity->get_component<physics::Transformation>()->local_inner_x_scale(width / img_dim.x);
}
