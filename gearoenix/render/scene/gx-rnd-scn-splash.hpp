#pragma once
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <optional>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::material {
struct Unlit;
}

namespace gearoenix::render::scene {
struct Splash final {
private:
    core::ecs::EntityPtr scene_entity;
    physics::Transformation* gear_tran = nullptr;
    physics::Transformation* glare_tran = nullptr;
    physics::Transformation* left_wing_tran = nullptr;
    physics::Transformation* right_wing_tran = nullptr;
    physics::Transformation* gearoenix_text_tran = nullptr;
    material::Unlit* gearoenix_text_mat = nullptr;
    double wings_current_angle = 0.0f;
    double current_scale = 0.0f;
    double scale_animation_time_current = 0.0;
    bool is_animating_scale = true;
    double text_animation_time_current = 0.0;
    bool is_animating_text = false;
    double after_animation = 0.0;
    std::weak_ptr<Splash> weak_self;
    std::optional<core::job::EndCaller<>> end_callback;

    [[nodiscard]] double calculate_scale() const;

    explicit Splash(core::job::EndCaller<>&& end_callback);
    void initialise(core::job::EndCaller<>&& start_callback);

public:
    [[nodiscard]] static std::shared_ptr<Splash> construct(core::job::EndCaller<>&& start_callback, core::job::EndCaller<>&& end_callback);
    void update();
    void hide();
};
}