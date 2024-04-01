#ifndef GEAROENIX_PHYSICS_ANIMATION_HPP
#define GEAROENIX_PHYSICS_ANIMATION_HPP
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

struct ArmatureAnimation final : public Animation {
    const std::size_t bones_channels_count = 0;
    const std::size_t bones_channels_first_index = 0;
    const std::size_t bones_channels_end_index = 0;

    ArmatureAnimation(
        std::string name,
        std::size_t bones_channels_count,
        std::size_t bones_channels_first_index,
        std::size_t bones_channels_end_index);
    ~ArmatureAnimation() final;
    void animate(Manager& manager, double time) final;
    void animate(Manager& manager, const BoneChannel& bone_channel, double time);
};

struct SpriteAnimation final : public Animation {
    const std::shared_ptr<render::material::Sprite> sprite;
    const double count;
    const std::size_t width;
    const std::size_t height;
    const math::Vec2<double> uv_scale;

    SpriteAnimation(
        std::string name,
        std::shared_ptr<render::material::Sprite> sprite,
        std::size_t width,
        std::size_t height);
    ~SpriteAnimation() final;
    void animate(Manager& manager, double time) final;
};

struct AnimationPlayer final : public core::ecs::Component {
    friend struct Manager;

    GX_GET_VAL_PRV(double, time, 0.0);
    GX_GETSET_VAL_PRV(double, speed, 1.0);
    GX_GETSET_VAL_PRV(bool, is_loop, true);
    GX_GET_VAL_PRV(double, loop_start_time, 0.1e-10);
    GX_GET_VAL_PRV(double, loop_end_time, 1.0);
    GX_GET_VAL_PRV(double, loop_length_time, loop_end_time - loop_start_time);
    GX_GET_REFC_PRV(std::shared_ptr<Animation>, animation);

public:
    AnimationPlayer(std::shared_ptr<Animation> animation, std::string&& name, double starting_time = 0.0);
    ~AnimationPlayer() final;
    [[nodiscard]] static std::shared_ptr<AnimationPlayer> construct(std::shared_ptr<Animation> animation, std::string&& name, double starting_time = 0.0);
    void update_time(double delta_time);
    void set_loop_start_time(double t);
    void set_loop_end_time(double t);
    void set_loop_range_time(double start, double end);
    void animate(Manager& manager);
    [[nodiscard]] const boost::container::flat_set<std::type_index>& get_all_the_hierarchy_types_except_component() const final;
};
}
#endif