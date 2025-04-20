#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-wdg-widget.hpp"
#include <optional>
#include <string>

namespace gearoenix::render::widget {
struct Button;
struct Modal final : Widget {
    struct ConstructorReturn final {
        core::ecs::EntityPtr background_model_builder;
        std::shared_ptr<Modal> modal;
    };

    GX_GET_CREF_PRV(std::shared_ptr<Button>, close);
    GX_GET_CREF_PRV(std::shared_ptr<Button>, cancel);
    GX_GET_CREF_PRV(std::shared_ptr<Button>, ok);

public:
    Modal(
        std::string&& name,
        std::shared_ptr<Button>&& close,
        std::shared_ptr<Button>&& cancel,
        std::shared_ptr<Button>&& ok);

    static void construct(
        std::string&& name,
        std::string&& background_texture_asset,
        const std::optional<std::pair<std::string /*pressed-texture-asset*/, std::string /*rest*/>>& close_button_texture_asset,
        const std::optional<std::pair<std::string /*pressed-texture-asset*/, std::string /*rest*/>>& cancel_button_texture_asset,
        const std::optional<std::pair<std::string /*pressed-texture-asset*/, std::string /*rest*/>>& ok_button_texture_asset,
        core::ecs::Entity* camera_entity,
        std::shared_ptr<Widget>&& parent,
        core::job::EndCaller<ConstructorReturn>&& end_callback);
    ~Modal() override;
};
}