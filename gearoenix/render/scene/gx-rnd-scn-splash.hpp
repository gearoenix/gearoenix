#ifndef GEAROENIX_RENDER_SCENE_SPLASH_HPP
#define GEAROENIX_RENDER_SCENE_SPLASH_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <optional>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::scene {
struct Splash final {
    engine::Engine& e;

private:
    core::ecs::entity_id_t scene_id = 0;
    core::ecs::entity_id_t gear_id = 0;
    core::ecs::entity_id_t glare_id = 0;
    core::ecs::entity_id_t left_wing_id = 0;
    core::ecs::entity_id_t right_wing_id = 0;
    double wings_current_angle = 0.0f;
    double current_scale = 0.0f;
    double scale_animation_time_current = 0.0;
    bool is_animating_scale = true;
    double text_animation_time_current = 0.0;
    bool is_animating_text = false;
    double after_animation = 0.0;
    core::ecs::entity_id_t gearoenix_text = 0;
    std::optional<core::sync::EndCaller> end_callback;

    [[nodiscard]] double calculate_scale() const noexcept;

public:
    Splash(engine::Engine& e, const core::sync::EndCaller& start_callback, const core::sync::EndCaller& end_callback) noexcept;
    void update() noexcept;
    void hide() noexcept;
};
}

#endif