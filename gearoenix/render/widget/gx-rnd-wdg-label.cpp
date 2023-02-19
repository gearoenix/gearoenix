#include "gx-rnd-wdg-label.hpp"
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

gearoenix::render::widget::Label::Label(std::string&& name, engine::Engine& e) noexcept
    : Widget(std::move(name), Type::Label, e)
{
}

gearoenix::render::widget::Label::~Label() noexcept = default;

std::tuple<std::shared_ptr<gearoenix::render::model::Builder>, std::shared_ptr<gearoenix::render::widget::Label>>
gearoenix::render::widget::Label::construct(
    std::string&& name,
    engine::Engine& e,
    const std::string& background_texture_asset,
    const core::ecs::entity_id_t camera_id,
    Widget* const parent,
    scene::Builder& scene_builder,
    const core::sync::EndCaller& end_callback) noexcept
{
    return construct(
        std::move(name), e,
        e.get_texture_manager()->create_2d_from_file(
            background_texture_asset,
            platform::stream::Path::create_asset(background_texture_asset),
            render::texture::TextureInfo(),
            end_callback),
        camera_id, parent, scene_builder, end_callback);
}

std::tuple<std::shared_ptr<gearoenix::render::model::Builder>, std::shared_ptr<gearoenix::render::widget::Label>>
gearoenix::render::widget::Label::construct(
    std::string&& name,
    engine::Engine& e,
    const std::shared_ptr<texture::Texture2D>& background_texture,
    const core::ecs::entity_id_t camera_id,
    Widget* const parent,
    scene::Builder& scene_builder,
    const core::sync::EndCaller& end_callback) noexcept
{
    auto mat = e.get_material_manager()->get_unlit(name + "-material", end_callback);
    mat->set_transparency(render::material::Transparency::Transparent);
    mat->set_albedo(background_texture);
    auto model_builder = e.get_model_manager()->build(
        name + "-model",
        e.get_mesh_manager()->build_plate(end_callback),
        std::shared_ptr(mat),
        core::sync::EndCaller(end_callback),
        true);
    scene_builder.add(std::shared_ptr(model_builder));
    auto result = std::make_shared<Label>(std::move(name), e);
    result->set_model_entity_id(model_builder->get_id());
    result->set_camera_entity_id(camera_id);
    if (nullptr != parent)
        parent->add_child(result);
    return { std::move(model_builder), std::move(result) };
}
