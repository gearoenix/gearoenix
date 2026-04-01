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
    core::fp_t wings_current_angle = static_cast<core::fp_t>(0);
    core::fp_t current_scale = static_cast<core::fp_t>(0);
    core::fp_t scale_animation_time_current = static_cast<core::fp_t>(0);
    bool is_animating_scale = true;
    core::fp_t text_animation_time_current = static_cast<core::fp_t>(0);
    bool is_animating_text = false;
    core::fp_t after_animation = static_cast<core::fp_t>(0);
    std::weak_ptr<Splash> weak_self;
    std::optional<core::job::EndCaller<>> end_callback;

    explicit Splash(core::job::EndCaller<>&& end_callback);
    void initialise(core::job::EndCaller<>&& start_callback);

public:
    [[nodiscard]] static std::shared_ptr<Splash> construct(core::job::EndCaller<>&& start_callback, core::job::EndCaller<>&& end_callback);
    void update();
    void hide();
};
}