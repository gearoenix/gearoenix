#ifndef GEAROENIX_RENDER_WIDGET_LABEL_HPP
#define GEAROENIX_RENDER_WIDGET_LABEL_HPP
#include "gx-rnd-wdg-widget.hpp"

namespace gearoenix::core::sync {
struct EndCaller;
}

namespace gearoenix::render::model {
struct Builder;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::widget {
struct Label final : public Widget {
public:
    Label(std::string&& name, engine::Engine& e) noexcept;
    ~Label() noexcept final;
    [[nodiscard]] static std::tuple<std::shared_ptr<model::Builder>, std::shared_ptr<Label>> construct(
        std::string&& name,
        engine::Engine& e,
        const std::string& background_texture_asset,
        core::ecs::entity_id_t camera_id,
        Widget* parent,
        scene::Builder& scene_builder,
        const core::sync::EndCaller& end_callback) noexcept;
    [[nodiscard]] static std::tuple<std::shared_ptr<model::Builder>, std::shared_ptr<Label>> construct(
        std::string&& name,
        engine::Engine& e,
        const std::shared_ptr<texture::Texture2D>& background_texture,
        core::ecs::entity_id_t camera_id,
        Widget* parent,
        scene::Builder& scene_builder,
        const core::sync::EndCaller& end_callback) noexcept;
};
}
#endif
