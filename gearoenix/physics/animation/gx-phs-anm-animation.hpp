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

struct Animation : core::Object {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = 16;
    constexpr static auto object_type_index = gearoenix_physics_animation_type_index;

    explicit Animation(core::object_type_index_t, std::string&& name);
    ~Animation() override;
    virtual void animate(core::fp_t time) = 0;
    virtual void write(platform::stream::Stream& s) const;
    [[nodiscard]] virtual math::Vec2<core::fp_t> get_start_end() const = 0;
};

struct ArmatureAnimation final : Animation {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto object_type_index = gearoenix_physics_animation_armature_animation_type_index;
    constexpr static std::array all_parent_object_type_indices { Animation::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Animation::object_type_index };

    /// Root bones that the animation applies on them
    const std::shared_ptr<Bone> root_bone;

    ArmatureAnimation(std::string&& name, std::shared_ptr<Bone>&& root_bone);

    ~ArmatureAnimation() override;
    void animate(core::fp_t time) override;
    static void animate(const Bone& bone, core::fp_t time);
    void show_debug_gui() override;
    void write(platform::stream::Stream& s) const override;
    [[nodiscard]] math::Vec2<core::fp_t> get_start_end() const override;
};

struct SpriteAnimation final : Animation {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto object_type_index = gearoenix_physics_animation_sprite_animation_type_index;
    constexpr static std::array all_parent_object_type_indices { Animation::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Animation::object_type_index };

    const std::shared_ptr<render::material::Sprite> sprite;
    const core::fp_t count;
    const math::Vec2<std::uint32_t> aspect;
    const math::Vec2<core::fp_t> uv_scale;

    SpriteAnimation(std::string&& name, std::shared_ptr<render::material::Sprite>&& sprite, std::uint32_t width, std::uint32_t height);
    ~SpriteAnimation() override;
    void animate(core::fp_t time) override;
    void show_debug_gui() override;
    void write(platform::stream::Stream& s) const override;
    [[nodiscard]] math::Vec2<core::fp_t> get_start_end() const override;
};

struct AnimationPlayer final : core::ecs::Component {
    GEAROENIX_OBJECT_STRUCT_DEF;

    friend struct Manager;

    constexpr static auto max_count = 16;
    constexpr static auto object_type_index = gearoenix_physics_animation_player_type_index;

    GX_GET_VAL_PRV(core::fp_t, time, 0.0);
    GX_GETSET_VAL_PRV(core::fp_t, speed, 1.0);
    GX_GETSET_VAL_PRV(bool, is_loop, true);
    GX_GET_VAL_PRV(core::fp_t, loop_start_time, 0.1e-10);
    GX_GET_VAL_PRV(core::fp_t, loop_end_time, 1.0);
    GX_GET_VAL_PRV(core::fp_t, loop_length_time, loop_end_time - loop_start_time);
    GX_GET_REFC_PRV(std::shared_ptr<Animation>, animation);

    AnimationPlayer(core::ecs::Entity* entity, std::shared_ptr<Animation>&& animation, std::string&& name, core::fp_t starting_time);

    void show_debug_gui() override;

public:
    ~AnimationPlayer() override;
    void update_time(core::fp_t delta_time);
    void set_loop_start_time(core::fp_t t);
    void set_loop_end_time(core::fp_t t);
    void set_loop_range_time(core::fp_t start, core::fp_t end);
    void animate() const;
    // void write_in_io_context(std::shared_ptr<platform::stream::Stream> &&, core::job::EndCaller<> &&) const override;
    // void update_in_io_context(std::shared_ptr<platform::stream::Stream> &&, core::job::EndCaller<> &&) override;
};
}