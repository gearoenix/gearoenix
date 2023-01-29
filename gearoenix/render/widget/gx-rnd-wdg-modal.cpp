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
    std::shared_ptr<Button>&& ok) noexcept
    : Widget(std::move(name), Type::Modal, e)
    , close(std::move(close))
    , cancel(std::move(cancel))
    , ok(std::move(ok))
{
}

std::tuple<
    std::shared_ptr<gearoenix::render::model::Builder>,
    std::shared_ptr<gearoenix::render::model::Builder>,
    std::shared_ptr<gearoenix::render::model::Builder>,
    std::shared_ptr<gearoenix::render::model::Builder>,
    std::shared_ptr<gearoenix::render::widget::Modal>>
gearoenix::render::widget::Modal::construct(
    std::string&& n,
    engine::Engine& e,
    const std::string& background_texture_asset,
    const std::optional<std::pair<std::string, std::string>>& close_fs,
    const std::optional<std::pair<std::string, std::string>>& cancel_fs,
    const std::optional<std::pair<std::string, std::string>>& ok_fs,
    core::ecs::entity_id_t camera_id,
    Widget* const parent,
    scene::Builder& scene_builder,
    const core::sync::EndCaller& end_callback) noexcept
{
    auto result = std::make_shared<Modal>(std::move(n), e, nullptr, nullptr, nullptr);
    auto mat = e.get_material_manager()->get_unlit(result->name + "-material", end_callback);
    mat->set_transparency(render::material::Transparency::Transparent);
    mat->set_albedo(e.get_texture_manager()->create_2d_from_file(
        background_texture_asset,
        platform::stream::Path::create_asset(background_texture_asset),
        render::texture::TextureInfo(),
        end_callback));
    auto bg_md = e.get_model_manager()->build(
        result->name + "-background-model",
        e.get_mesh_manager()->build_plate(end_callback),
        std::move(mat),
        core::sync::EndCaller(end_callback),
        true);
    result->set_model_entity_id(bg_md->get_id());
    result->set_camera_entity_id(camera_id);
    if (nullptr != parent)
        parent->add_child(result);
    scene_builder.add(std::shared_ptr(bg_md));
    auto [close_md, close_button] = close_fs.has_value() ? Button::construct(result->name + "-close-button", e, close_fs->first, close_fs->second, camera_id, *result, scene_builder, end_callback) : std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Button>>(nullptr, nullptr);
    auto [cancel_md, cancel_button] = cancel_fs.has_value() ? Button::construct(result->name + "-cancel-button", e, cancel_fs->first, cancel_fs->second, camera_id, *result, scene_builder, end_callback) : std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Button>>(nullptr, nullptr);
    auto [ok_md, ok_button] = ok_fs.has_value() ? Button::construct(result->name + "-ok-button", e, ok_fs->first, ok_fs->second, camera_id, *result, scene_builder, end_callback) : std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Button>>(nullptr, nullptr);
    result->close = std::move(close_button);
    result->cancel = std::move(cancel_button);
    result->ok = std::move(ok_button);
    return { std::move(bg_md), std::move(close_md), std::move(cancel_md), std::move(ok_md), std::move(result) };
}

gearoenix::render::widget::Modal::~Modal() noexcept = default;
