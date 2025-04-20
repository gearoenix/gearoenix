#include "gx-rnd-wdg-modal.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-wdg-button.hpp"

gearoenix::render::widget::Modal::Modal(
    std::string&& name,
    std::shared_ptr<Button>&& close,
    std::shared_ptr<Button>&& cancel,
    std::shared_ptr<Button>&& ok)
    : Widget(std::move(name), Type::Modal)
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
    core::ecs::Entity* const camera_entity,
    std::shared_ptr<Widget>&& parent,
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

    auto values = std::make_shared<Values>(std::move(end_callback));
    values->return_value.modal = std::make_shared<Modal>(std::move(name), nullptr, nullptr, nullptr);

    auto values_ready = core::job::EndCaller([values, parent = std::move(parent), camera_entity]() mutable {
        values->mat->set_transparency(material::Transparency::Transparent);
        values->mat->set_albedo(std::move(values->background_texture));
        values->return_value.background_model_builder = model::Manager::get().build(
            values->return_value.modal->name + "-background-model",
            parent ? parent->get_model_entity().get() : nullptr, { values->msh }, true);
        values->return_value.modal->transform = values->return_value.background_model_builder->get_component_shared_ptr<physics::Transformation>();
        values->return_value.modal->set_model_entity(core::ecs::EntityPtr(values->return_value.background_model_builder));
        values->return_value.modal->set_camera_entity(camera_entity);
        if (nullptr != parent) {
            parent->add_child(values->return_value.modal);
        }
        values->return_value.modal->parent = std::move(parent);
        values->end_callback.set_return(std::move(values->return_value));
    });

    material::Manager::get().get_unlit(
        values->return_value.modal->name + "-material",
        core::job::EndCallerShared<material::Unlit>([values, values_ready](std::shared_ptr<material::Unlit>&& mat) {
            values->mat = std::move(mat);
            mesh::Manager::get().build_plate(
                std::shared_ptr<material::Material>(values->mat),
                core::job::EndCallerShared<mesh::Mesh>([values, values_ready](std::shared_ptr<mesh::Mesh>&& msh) {
                    values->msh = std::move(msh);
                    (void)values_ready;
                }));
        }));

    const auto path = platform::stream::Path::create_asset(std::move(background_texture_asset));
    texture::Manager::get().create_2d_from_file(
        path, texture::TextureInfo(),
        core::job::EndCallerShared<texture::Texture2D>([values, values_ready](std::shared_ptr<texture::Texture2D>&& tex) {
            values->background_texture = std::move(tex);
            (void)values_ready;
        }));

    auto& result = values->return_value.modal;

    if (close_button_texture_asset.has_value()) {
        Button::construct(
            result->name + "-close-button", std::string(close_button_texture_asset->first),
            std::string(close_button_texture_asset->second), camera_entity,
            std::shared_ptr(result),
            core::job::EndCallerShared<Button>([values, values_ready](auto&& button) {
                values->return_value.modal->close = std::move(button);
                (void)values_ready;
            }));
    }

    if (cancel_button_texture_asset.has_value()) {
        Button::construct(
            result->name + "-cancel-button",
            std::string(cancel_button_texture_asset->first),
            std::string(cancel_button_texture_asset->second),
            camera_entity,
            std::shared_ptr(result),
            core::job::EndCallerShared<Button>([values, values_ready](std::shared_ptr<Button>&& button) {
                values->return_value.modal->cancel = std::move(button);
                (void)values_ready;
            }));
    }

    if (ok_button_texture_asset.has_value()) {
        Button::construct(
            result->name + "-ok-button",
            std::string(ok_button_texture_asset->first),
            std::string(ok_button_texture_asset->second),
            camera_entity,
            std::shared_ptr(result),
            core::job::EndCallerShared<Button>([values, values_ready](auto&& button) {
                values->return_value.modal->ok = std::move(button);
                (void)values_ready;
            }));
    }
}

gearoenix::render::widget::Modal::~Modal() = default;
