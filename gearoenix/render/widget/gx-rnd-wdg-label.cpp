#include "gx-rnd-wdg-label.hpp"

#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../model/gx-rnd-mdl-builder.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"

gearoenix::render::widget::Label::Label(std::string&& name, engine::Engine& e)
    : Widget(std::move(name), Type::Label, e)
{
}

gearoenix::render::widget::Label::~Label() = default;

void gearoenix::render::widget::Label::construct(
    std::string&& name,
    std::string&& background_texture_asset,
    const core::ecs::entity_id_t camera_id,
    std::shared_ptr<Widget>&& parent,
    std::shared_ptr<scene::Builder>&& scene_builder,
    core::job::EndCaller<ConstructorReturn>&& end_callback)
{
    auto& e = scene_builder->e;
    const auto path = platform::stream::Path::create_asset(std::move(background_texture_asset));
    e.get_texture_manager()->create_2d_from_file(
        path,
        texture::TextureInfo(),
        core::job::EndCallerShared<texture::Texture2D>(
            [n = std::move(name), c = camera_id, p = std::move(parent), s = std::move(scene_builder), e = std::move(end_callback)](std::shared_ptr<texture::Texture2D>&& t) mutable {
                construct(std::move(n), std::move(t), c, std::move(p), std::move(s), std::move(e));
            }));
}

void gearoenix::render::widget::Label::construct(
    std::string&& name,
    std::shared_ptr<texture::Texture2D>&& background_texture,
    const core::ecs::entity_id_t camera_id,
    std::shared_ptr<Widget>&& parent,
    std::shared_ptr<scene::Builder>&& scene_builder,
    core::job::EndCaller<ConstructorReturn>&& end_callback)
{
    auto& e = scene_builder->e;
    const auto material_name = name + "-material";
    e.get_material_manager()->get_unlit(material_name, core::job::EndCallerShared<material::Unlit>([n = std::move(name), t = std::move(background_texture), c = camera_id, p = std::move(parent), s = std::move(scene_builder), e = std::move(end_callback)](std::shared_ptr<material::Unlit>&& m) mutable {
        construct(std::move(n), std::move(t), std::move(m), c, std::move(p), std::move(s), std::move(e));
    }));
}

void gearoenix::render::widget::Label::construct(
    std::string&& name,
    std::shared_ptr<texture::Texture2D>&& background_texture,
    std::shared_ptr<material::Material>&& mat,
    const core::ecs::entity_id_t camera_id,
    std::shared_ptr<Widget>&& parent,
    std::shared_ptr<scene::Builder>&& scene_builder,
    core::job::EndCaller<ConstructorReturn>&& end_callback)
{
    auto& e = scene_builder->e;
    auto copy_mat = std::shared_ptr(mat);
    e.get_mesh_manager()->build_plate(
        std::move(copy_mat),
        core::job::EndCallerShared<mesh::Mesh>(
            [n = std::move(name), t = std::move(background_texture), m = std::move(mat), c = camera_id, p = std::move(parent), s = std::move(scene_builder), e = std::move(end_callback)](std::shared_ptr<mesh::Mesh>&& msh) mutable {
                construct(std::move(n), std::move(t), std::move(m), std::move(msh), c, std::move(p), std::move(s), std::move(e));
            }));
}

void gearoenix::render::widget::Label::construct(
    std::string&& name,
    std::shared_ptr<texture::Texture2D>&& background_texture,
    std::shared_ptr<material::Material>&& mat,
    std::shared_ptr<mesh::Mesh>&& msh,
    const core::ecs::entity_id_t camera_id,
    std::shared_ptr<Widget>&& parent,
    std::shared_ptr<scene::Builder>&& scene_builder,
    core::job::EndCaller<ConstructorReturn>&& end_callback)
{
    mat->set_transparency(material::Transparency::Transparent);
    mat->set_albedo(std::move(background_texture));
    auto& e = scene_builder->e;
    auto model_builder = e.get_model_manager()->build(
        name + "-model",
        parent ? parent->get_transform().get() : nullptr,
        { std::move(msh) },
        core::job::EndCaller([] { }),
        true);
    scene_builder->add(std::shared_ptr(model_builder));
    auto result = std::make_shared<Label>(std::move(name), e);
    result->transform = std::dynamic_pointer_cast<physics::Transformation>(model_builder->get_transformation().get_component_self().lock());
    result->set_model_entity_id(model_builder->get_id());
    result->set_camera_entity_id(camera_id);
    if (nullptr != parent) {
        parent->add_child(result);
    }
    result->parent = std::move(parent);
    end_callback.set_return({ std::move(model_builder), std::move(result) });
}
