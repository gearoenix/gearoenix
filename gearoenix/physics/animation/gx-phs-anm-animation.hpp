#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-phs-anm-channel.hpp"
#include <string>
#include <vector>

namespace gearoenix::render::material {
struct Sprite;
}

namespace gearoenix::physics::animation {
struct Manager;
struct ArmatureAnimationInfo final {
    std::string name;
    std::vector<BoneChannelBuilder> channels;

    void optimise();
};

struct Animation {
    const std::string name;

    explicit Animation(std::string name);
    virtual ~Animation();
    virtual void animate(Manager& manager, double time) = 0;
};

struct ArmatureAnimation final : Animation {
    const std::uint32_t bones_channels_count = 0;
    const std::uint32_t bones_channels_first_index = 0;
    const std::uint32_t bones_channels_end_index = 0;

    ArmatureAnimation(
        std::string name,
        std::uint32_t bones_channels_count,
        std::uint32_t bones_channels_first_index,
        std::uint32_t bones_channels_end_index);
    ~ArmatureAnimation() override;
    void animate(Manager& manager, double time) override;
    void animate(Manager& manager, const BoneChannel& bone_channel, double time);
};

struct SpriteAnimation final : Animation {
    const std::shared_ptr<render::material::Sprite> sprite;
    const double count;
    const std::uint32_t width;
    const std::uint32_t height;
    const math::Vec2<double> uv_scale;

    SpriteAnimation(
        std::string name,
        std::shared_ptr<render::material::Sprite> sprite,
        std::uint32_t width,
        std::uint32_t height);
    ~SpriteAnimation() override;
    void animate(Manager& manager, double time) override;
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

public:
    AnimationPlayer(std::shared_ptr<Animation> animation, std::string&& name, double starting_time, core::ecs::entity_id_t entity_id);
    ~AnimationPlayer() override;
    void update_time(double delta_time);
    void set_loop_start_time(double t);
    void set_loop_end_time(double t);
    void set_loop_range_time(double start, double end);
    void animate(Manager& manager);
};
}