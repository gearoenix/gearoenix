#ifndef GEAROENIX_PHYSICS_ANIMATION_INTERPOLATION_HPP
#define GEAROENIX_PHYSICS_ANIMATION_INTERPOLATION_HPP
#include "gx-phs-anm-keyframe.hpp"

namespace gearoenix::physics::animation {

enum struct Interpolation : std::uint8_t {
    Step = 11,
    Linear = 23,
    Gltf2CubicSPLine = 37,
};

template <typename Value, typename T>
[[nodiscard]] constexpr Value interpolate(
    const std::pair<double, Keyframe<Value>>& start_keyframe,
    const std::pair<double, Keyframe<Value>>& end_keyframe,
    const T time) noexcept
{
    const auto td = end_keyframe.first - start_keyframe.first;
    const auto t = (time - start_keyframe.first) / td;
    if (std::holds_alternative<KeyframeGltf2Bezier<Value>>(start_keyframe.second) && std::holds_alternative<KeyframeGltf2Bezier<Value>>(end_keyframe.second)) {
        const auto& sk = std::get<KeyframeGltf2Bezier<Value>>(start_keyframe.second);
        const auto& ek = std::get<KeyframeGltf2Bezier<Value>>(end_keyframe.second);
        const auto tp2 = t * t;
        const auto tp3 = tp2 * t;
        const auto tp2mp3 = tp2 * 3.0;
        return (sk.key * ((tp3 * 2.0) - tp2mp3 + 1.0)) + (sk.out * (td * (tp3 - (tp2 * 2.0) + t))) + (ek.key * ((-2.0 * tp3) + tp2mp3)) + (ek.in * (td * (tp3 - tp2)));
    }
    if (std::holds_alternative<KeyframeStep<Value>>(start_keyframe.second) || std::holds_alternative<KeyframeStep<Value>>(end_keyframe.second)) {
        if (t > 0.5)
            return get_key(end_keyframe.second);
        return get_key(start_keyframe.second);
    }
    const auto& sk = get_key(start_keyframe.second);
    const auto& ek = get_key(end_keyframe.second);
    return (sk * t) + (ek * (1.0 - t));
}
}

#endif