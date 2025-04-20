#include "gx-rnd-wdg-label.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"

gearoenix::render::widget::Label::Label(std::string&& name)
    : Widget(std::move(name), Type::Label)
{
}

gearoenix::render::widget::Label::~Label() = default;

void gearoenix::render::widget::Label::construct(
    std::string&& name,
    std::string&& background_texture_asset,
    core::ecs::Entity* const camera_entity,
    std::shared_ptr<Widget>&& parent,
    core::job::EndCallerShared<Label>&& end_callback)
{
    const auto path = platform::stream::Path::create_asset(std::move(background_texture_asset));
    texture::Manager::get().create_2d_from_file(
        path, texture::TextureInfo(),
        core::job::EndCallerShared<texture::Texture2D>(
            [n = std::move(name), c = camera_entity, p = std::move(parent), e = std::move(end_callback)](std::shared_ptr<texture::Texture2D>&& t) mutable {
                construct(std::move(n), std::move(t), c, std::move(p), std::move(e));
            }));
}

void gearoenix::render::widget::Label::construct(
    std::string&& name,
    std::shared_ptr<texture::Texture2D>&& background_texture,
    core::ecs::Entity* const camera_entity,
    std::shared_ptr<Widget>&& parent,
    core::job::EndCallerShared<Label>&& end_callback)
{
    const auto material_name = name + "-material";
    material::Manager::get().get_unlit(material_name, core::job::EndCallerShared<material::Unlit>([n = std::move(name), t = std::move(background_texture), c = camera_entity, p = std::move(parent), e = std::move(end_callback)](std::shared_ptr<material::Unlit>&& m) mutable {
        construct(std::move(n), std::move(t), std::move(m), c, std::move(p), std::move(e));
    }));
}

void gearoenix::render::widget::Label::construct(
    std::string&& name,
    std::shared_ptr<texture::Texture2D>&& background_texture,
    std::shared_ptr<material::Material>&& mat,
    core::ecs::Entity* const camera_entity,
    std::shared_ptr<Widget>&& parent,
    core::job::EndCallerShared<Label>&& end_callback)
{
    auto copy_mat = std::shared_ptr(mat);
    mesh::Manager::get().build_plate(
        std::move(copy_mat),
        core::job::EndCallerShared<mesh::Mesh>(
            [n = std::move(name), t = std::move(background_texture), m = std::move(mat), c = camera_entity, p = std::move(parent), e = std::move(end_callback)](std::shared_ptr<mesh::Mesh>&& msh) mutable {
                construct(std::move(n), std::move(t), std::move(m), std::move(msh), c, std::move(p), std::move(e));
            }));
}

void gearoenix::render::widget::Label::construct(
    std::string&& name,
    std::shared_ptr<texture::Texture2D>&& background_texture,
    std::shared_ptr<material::Material>&& mat,
    std::shared_ptr<mesh::Mesh>&& msh,
    core::ecs::Entity* const camera_entity,
    std::shared_ptr<Widget>&& parent,
    core::job::EndCallerShared<Label>&& end_callback)
{
    mat->set_transparency(material::Transparency::Transparent);
    mat->set_albedo(std::move(background_texture));
    auto model_entity = model::Manager::get().build(name + "-model", parent->get_model_entity().get(), { std::move(msh) }, true);
    auto result = std::make_shared<Label>(std::move(name));
    result->transform = model_entity->get_component_shared_ptr<physics::Transformation>();
    result->set_model_entity(std::move(model_entity));
    result->set_camera_entity(camera_entity);
    if (nullptr != parent) {
        parent->add_child(result);
    }
    result->parent = std::move(parent);
    end_callback.set_return(std::move(result));
}
