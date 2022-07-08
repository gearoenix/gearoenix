#ifndef GEAROENIX_PHYSICS_ANIMATION_CHANNEL_HPP
#define GEAROENIX_PHYSICS_ANIMATION_CHANNEL_HPP
#include "../../math/gx-math-quaternion.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include <string>
#include <variant>

namespace gearoenix::physics::animation {
struct Manager;

template <typename Value>
struct KeyframeBezier final {
    Value key;
    Value in;
    Value out;
};

template <typename Value>
struct KeyframeLinear final {
    Value key;
};

template <typename Value>
using Keyframe = std::variant<KeyframeLinear<Value>, KeyframeBezier<Value>>;

template <typename Value>
[[nodiscard]] constexpr const Value& get_key(const Keyframe<Value>& keyframe) noexcept
{
    if (std::holds_alternative<KeyframeLinear<Value>>(keyframe))
        return std::get<KeyframeLinear<Value>>(keyframe).key;
    return std::get<KeyframeBezier<Value>>(keyframe).key;
}

template <typename Value, typename T>
[[nodiscard]] Value interpolate_keyframes(
    const std::pair<double, Keyframe<Value>>& start_keyframe,
    const std::pair<double, Keyframe<Value>>& end_keyframe,
    const T time) noexcept
{
    const auto td = end_keyframe.first - start_keyframe.first;
    const auto t = (time - start_keyframe.first) / td;
    if (std::holds_alternative<KeyframeBezier<Value>>(start_keyframe.second) && std::holds_alternative<KeyframeBezier<Value>>(end_keyframe.second)) {
        const auto& sk = std::get<KeyframeBezier<Value>>(start_keyframe.second);
        const auto& ek = std::get<KeyframeBezier<Value>>(end_keyframe.second);
        const auto tp2 = t * t;
        const auto tp3 = tp2 * t;
        const auto tp2mp3 = tp2 * 3.0;
        return (sk.key * ((tp3 * 2.0) - tp2mp3 + 1.0)) + (sk.out * (td * (tp3 - (tp2 * 2.0) + t))) + (ek.key * ((-2.0 * tp3) + tp2mp3)) + (ek.in * (td * (tp3 - tp2)));
    }
    GX_UNIMPLEMENTED;
}

struct BoneChannelBuilder final {
    std::string target_bone;
    std::vector<std::pair<double, Keyframe<math::Vec3<double>>>> scale_samples;
    std::vector<std::pair<double, Keyframe<math::Quat<double>>>> rotation_samples;
    std::vector<std::pair<double, Keyframe<math::Vec3<double>>>> translation_samples;
};

struct BoneChannel final {
    std::size_t target_bone_index = static_cast<std::size_t>(-1);
    std::size_t scale_samples_count = static_cast<std::size_t>(-1);
    std::size_t scale_samples_first_keyframe_index = static_cast<std::size_t>(-1);
    std::size_t scale_samples_end_keyframe_index = static_cast<std::size_t>(-1);
    std::size_t rotation_samples_count = static_cast<std::size_t>(-1);
    std::size_t rotation_samples_first_keyframe_index = static_cast<std::size_t>(-1);
    std::size_t rotation_samples_end_keyframe_index = static_cast<std::size_t>(-1);
    std::size_t translation_samples_count = static_cast<std::size_t>(-1);
    std::size_t translation_samples_first_keyframe_index = static_cast<std::size_t>(-1);
    std::size_t translation_samples_end_keyframe_index = static_cast<std::size_t>(-1);
};
}
#endif