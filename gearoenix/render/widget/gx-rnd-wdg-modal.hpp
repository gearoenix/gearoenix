#ifndef GEAROENIX_RENDER_WIDGET_MODAL_HPP
#define GEAROENIX_RENDER_WIDGET_MODAL_HPP
#include "gx-rnd-wdg-widget.hpp"

#include <functional>
#include <optional>
#include <string>
#include <tuple>

namespace gearoenix::core::sync {
struct EndCaller;
}

namespace gearoenix::render::model {
struct Builder;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::widget {
struct Button;
struct Modal final : public Widget {
    GX_GET_CREF_PRV(std::shared_ptr<Button>, close);
    GX_GET_CREF_PRV(std::shared_ptr<Button>, cancel);
    GX_GET_CREF_PRV(std::shared_ptr<Button>, ok);

public:
    Modal(
        std::string&& name,
        engine::Engine& e,
        std::shared_ptr<Button>&& close,
        std::shared_ptr<Button>&& cancel,
        std::shared_ptr<Button>&& ok) noexcept;
    [[nodiscard]] static std::tuple<
        std::shared_ptr<model::Builder> /*background-model-builder*/,
        std::shared_ptr<model::Builder> /*close-button-model-builder*/,
        std::shared_ptr<model::Builder> /*cancel-button-model-builder*/,
        std::shared_ptr<model::Builder> /*ok-button-model-builder*/,
        std::shared_ptr<Modal>>
    construct(
        std::string&& name,
        engine::Engine& e,
        const std::string& background_texture_asset,
        const std::optional<std::pair<std::string /*pressed-texture-asset*/, std::string /*rest*/>>& close_button_texture_asset,
        const std::optional<std::pair<std::string /*pressed-texture-asset*/, std::string /*rest*/>>& cancel_button_texture_asset,
        const std::optional<std::pair<std::string /*pressed-texture-asset*/, std::string /*rest*/>>& ok_button_texture_asset,
        core::ecs::entity_id_t camera_id,
        Widget* parent,
        scene::Builder& scene_builder,
        const core::sync::EndCaller& end_callback) noexcept;
    ~Modal() noexcept final;
};
}
#endif
