#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vector-2d.hpp"
#include <memory>
#include <string>
#include <vector>

namespace gearoenix::render::material {
struct Sprite;
}

namespace gearoenix::physics::animation {
struct Manager;
struct Bone;

struct Animation {
    const std::string name;

    explicit Animation(std::string&& name);
    virtual ~Animation();
    virtual void animate(double time) = 0;
    virtual void show_debug_gui();
};

struct ArmatureAnimation final : Animation {
    /// Root bones that the animation applies on them
    const std::shared_ptr<Bone> root_bone;

    ArmatureAnimation(std::string&& name, std::shared_ptr<Bone>&& root_bone);
    ~ArmatureAnimation() override;
    void animate(double time) override;
    static void animate(const Bone& bone, double time);
    void show_debug_gui() override;
};

struct SpriteAnimation final : Animation {
    const std::shared_ptr<render::material::Sprite> sprite;
    const double count;
    const math::Vec2<std::uint32_t> aspect;
    const math::Vec2<double> uv_scale;

    SpriteAnimation(
        std::string&& name,
        std::shared_ptr<render::material::Sprite>&& sprite,
        std::uint32_t width,
        std::uint32_t height);
    ~SpriteAnimation() override;
    void animate(double time) override;
    void show_debug_gui() override;
};

struct AnimationPlayer final : core::ecs::Component {
    friend struct Manager;
    constexpr static std::uint32_t MAX_COUNT = 16;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 5;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    GX_GET_VAL_PRV(double, time, 0.0);
    GX_GETSET_VAL_PRV(double, speed, 1.0);
    GX_GETSET_VAL_PRV(bool, is_loop, true);
    GX_GET_VAL_PRV(double, loop_start_time, 0.1e-10);
    GX_GET_VAL_PRV(double, loop_end_time, 1.0);
    GX_GET_VAL_PRV(double, loop_length_time, loop_end_time - loop_start_time);
    GX_GET_REFC_PRV(std::shared_ptr<Animation>, animation);

    void show_debug_gui() override;

public:
    AnimationPlayer(std::shared_ptr<Animation>&& animation, std::string&& name, double starting_time, core::ecs::entity_id_t entity_id);
    ~AnimationPlayer() override;
    void update_time(double delta_time);
    void set_loop_start_time(double t);
    void set_loop_end_time(double t);
    void set_loop_range_time(double start, double end);
    void animate() const;
};
}