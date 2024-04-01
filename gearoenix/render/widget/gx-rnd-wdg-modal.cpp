#include "gx-rnd-wdg-modal.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../model/gx-rnd-mdl-builder.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-wdg-button.hpp"

gearoenix::render::widget::Modal::Modal(
    std::string&& name,
    engine::Engine& e,
    std::shared_ptr<Button>&& close,
    std::shared_ptr<Button>&& cancel,
    std::shared_ptr<Button>&& ok)
    : Widget(std::move(name), Type::Modal, e)
    , close(std::move(close))
    , cancel(std::move(cancel))
    , ok(std::move(ok))
{
}

void gearoenix::render::widget::Modal::construct(
    std::string&& name,
    std::string&& background_texture_asset,
    const std::optional<std::pair<std::string, std::string>>& close_button_texture_asset,
    const std::optional<std::pair<std::string, std::string>>& cancel_button_texture_asset,
    const std::optional<std::pair<std::string, std::string>>& ok_button_texture_asset,
    const core::ecs::entity_id_t camera_id,
    std::shared_ptr<Widget>&& parent,
    std::shared_ptr<scene::Builder>&& scene_builder,
    core::job::EndCaller<ConstructorReturn>&& end_callback)
{
    struct Values {
        core::job::EndCaller<ConstructorReturn> end_callback;
        ConstructorReturn return_value;
        std::shared_ptr<material::Unlit> mat;
        std::shared_ptr<texture::Texture2D> background_texture;
        std::shared_ptr<mesh::Mesh> msh;

        explicit Values(core::job::EndCaller<ConstructorReturn>&& end_callback)
            : end_callback(std::move(end_callback))
        {
        }
    };

    auto& e = scene_builder->e;

    auto values = std::make_shared<Values>(std::move(end_callback));
    values->return_value.modal = std::make_shared<Modal>(std::move(name), e, nullptr, nullptr, nullptr);

    auto values_ready = core::job::EndCaller([values, parent = std::move(parent), scene_builder = scene_builder, camera_id]() mutable {
        values->mat->set_transparency(material::Transparency::Transparent);
        values->mat->set_albedo(std::move(values->background_texture));
        values->return_value.background_model_builder = values->return_value.modal->e.get_model_manager()->build(
            values->return_value.modal->name + "-background-model",
            std::move(values->msh),
            std::move(values->mat),
            core::job::EndCaller([] {}),
            true);
        values->return_value.modal->set_model_entity_id(values->return_value.background_model_builder->get_id());
        values->return_value.modal->set_camera_entity_id(camera_id);
        if (nullptr != parent) {
            parent->add_child(values->return_value.modal);
        }
        values->return_value.modal->parent = std::move(parent);
        scene_builder->add(std::shared_ptr(values->return_value.background_model_builder));
        values->end_callback.set_return(std::move(values->return_value));
    });

    e.get_material_manager()->get_unlit(
        values->return_value.modal->name + "-material",
        core::job::EndCallerShared<material::Unlit>([values, values_ready](std::shared_ptr<material::Unlit>&& mat) {
            values->mat = std::move(mat);
            (void)values_ready;
        }));

    const auto path = platform::stream::Path::create_asset(background_texture_asset);
    e.get_texture_manager()->create_2d_from_file(
        std::move(background_texture_asset), path, texture::TextureInfo(),
        core::job::EndCallerShared<texture::Texture2D>([values, values_ready](std::shared_ptr<texture::Texture2D>&& tex) {
            values->background_texture = std::move(tex);
            (void)values_ready;
        }));

    e.get_mesh_manager()->build_plate(core::job::EndCallerShared<mesh::Mesh>([values, values_ready](std::shared_ptr<mesh::Mesh>&& msh) {
        values->msh = std::move(msh);
        (void)values_ready;
    }));

    auto& result = values->return_value.modal;

    if (close_button_texture_asset.has_value()) {
        Button::construct(
            result->name + "-close-button", std::string(close_button_texture_asset->first), std::string(close_button_texture_asset->second), camera_id,
            std::shared_ptr(result), std::shared_ptr(scene_builder),
            core::job::EndCaller<std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Button>>>([values, values_ready](auto&& bb) {
                values->return_value.modal->close = std::move(bb.second);
                values->return_value.close_button_model_builder = std::move(bb.first);
                (void)values_ready;
            }));
    }

    if (cancel_button_texture_asset.has_value()) {
        Button::construct(
            result->name + "-cancel-button", std::string(cancel_button_texture_asset->first), std::string(cancel_button_texture_asset->second), camera_id,
            std::shared_ptr(result), std::shared_ptr(scene_builder),
            core::job::EndCaller<std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Button>>>([values, values_ready](auto&& bb) {
                values->return_value.modal->cancel = std::move(bb.second);
                values->return_value.cancel_button_model_builder = std::move(bb.first);
                (void)values_ready;
            }));
    }

    if (ok_button_texture_asset.has_value()) {
        Button::construct(
            result->name + "-ok-button", std::string(ok_button_texture_asset->first), std::string(ok_button_texture_asset->second), camera_id,
            std::shared_ptr(result), std::move(scene_builder),
            core::job::EndCaller<std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Button>>>([values, values_ready](auto&& bb) {
                values->return_value.modal->ok = std::move(bb.second);
                values->return_value.ok_button_model_builder = std::move(bb.first);
                (void)values_ready;
            }));
    }
}

gearoenix::render::widget::Modal::~Modal() = default;
