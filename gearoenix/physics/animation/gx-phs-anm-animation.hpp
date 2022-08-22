#ifndef GEAROENIX_PHYSICS_ANIMATION_HPP
#define GEAROENIX_PHYSICS_ANIMATION_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-phs-anm-channel.hpp"
#include <string>
#include <typeindex>
#include <vector>

namespace gearoenix::physics::animation {
struct Manager;
struct ArmatureAnimationInfo final {
    std::string name;
    std::vector<BoneChannelBuilder> channels;

    void optimise() noexcept;
};

struct ArmatureAnimation final {
    std::string name;
    std::size_t bones_channels_count = 0;
    std::size_t bones_channels_first_index = 0;
    std::size_t bones_channels_end_index = 0;
};

struct AnimationPlayer final : core::ecs::Component {
    friend struct Manager;

    GX_GET_VAL_PRV(std::size_t, index, static_cast<std::size_t>(-1));
    GX_GET_VAL_PRV(double, time, 0.0)
    GX_GETSET_VAL_PRV(bool, is_loop, true)
    GX_GET_VAL_PRV(double, loop_start_time, 0.1e-10)
    GX_GET_VAL_PRV(double, loop_end_time, 1.0)
    GX_GET_VAL_PRV(double, loop_length_time, loop_end_time - loop_start_time)
    GX_GET_VAL_PRV(std::type_index, animation_type, std::type_index(typeid(ArmatureAnimation)))

    explicit AnimationPlayer(std::size_t index, std::type_index animation_type = std::type_index(typeid(ArmatureAnimation)), double starting_time = 0.0) noexcept;
    AnimationPlayer(AnimationPlayer&&) noexcept;
    ~AnimationPlayer() noexcept final;

    void update_time(double delta_time) noexcept;
    void set_loop_start_time(double t) noexcept;
    void set_loop_end_time(double t) noexcept;
    void set_loop_range_time(double start, double end) noexcept;
};
}
#endif