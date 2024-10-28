#ifndef GEAROENIX_RENDER_SCENE_SPLASH_HPP
#define GEAROENIX_RENDER_SCENE_SPLASH_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <optional>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::scene {
struct Splash final {
    engine::Engine& e;

private:
    core::ecs::entity_id_t scene_id = core::ecs::INVALID_ENTITY_ID;
    core::ecs::entity_id_t gear_id = core::ecs::INVALID_ENTITY_ID;
    core::ecs::entity_id_t glare_id = core::ecs::INVALID_ENTITY_ID;
    core::ecs::entity_id_t left_wing_id = core::ecs::INVALID_ENTITY_ID;
    core::ecs::entity_id_t right_wing_id = core::ecs::INVALID_ENTITY_ID;
    double wings_current_angle = 0.0f;
    double current_scale = 0.0f;
    double scale_animation_time_current = 0.0;
    bool is_animating_scale = true;
    double text_animation_time_current = 0.0;
    bool is_animating_text = false;
    double after_animation = 0.0;
    core::ecs::entity_id_t gearoenix_text = core::ecs::INVALID_ENTITY_ID;
    std::weak_ptr<Splash> weak_self;
    std::optional<core::job::EndCaller<>> end_callback;

    [[nodiscard]] double calculate_scale() const;

    Splash(engine::Engine& e, const core::job::EndCaller<>& end_callback);
    void initialise(const core::job::EndCaller<>& start_callback);

public:
    [[nodiscard]] static std::shared_ptr<Splash> construct(engine::Engine& e, const core::job::EndCaller<>& start_callback, const core::job::EndCaller<>& end_callback);
    void update();
    void hide();
};
}

#endif