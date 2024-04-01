#ifndef GEAROENIX_RENDER_WIDGET_MODAL_HPP
#define GEAROENIX_RENDER_WIDGET_MODAL_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-wdg-widget.hpp"
#include <optional>
#include <string>

namespace gearoenix::render::model {
struct Builder;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::widget {
struct Button;
struct Modal final : Widget {
    struct ConstructorReturn {
        std::shared_ptr<model::Builder> background_model_builder;
        std::shared_ptr<model::Builder> close_button_model_builder;
        std::shared_ptr<model::Builder> cancel_button_model_builder;
        std::shared_ptr<model::Builder> ok_button_model_builder;
        std::shared_ptr<Modal> modal;
    };

    GX_GET_CREF_PRV(std::shared_ptr<Button>, close);
    GX_GET_CREF_PRV(std::shared_ptr<Button>, cancel);
    GX_GET_CREF_PRV(std::shared_ptr<Button>, ok);

public:
    Modal(
        std::string&& name,
        engine::Engine& e,
        std::shared_ptr<Button>&& close,
        std::shared_ptr<Button>&& cancel,
        std::shared_ptr<Button>&& ok);
    static void construct(
        std::string&& name,
        std::string&& background_texture_asset,
        const std::optional<std::pair<std::string /*pressed-texture-asset*/, std::string /*rest*/>>& close_button_texture_asset,
        const std::optional<std::pair<std::string /*pressed-texture-asset*/, std::string /*rest*/>>& cancel_button_texture_asset,
        const std::optional<std::pair<std::string /*pressed-texture-asset*/, std::string /*rest*/>>& ok_button_texture_asset,
        core::ecs::entity_id_t camera_id,
        std::shared_ptr<Widget>&& parent,
        std::shared_ptr<scene::Builder>&& scene_builder,
        core::job::EndCaller<ConstructorReturn>&& end_callback);
    ~Modal() override;
};
}
#endif
