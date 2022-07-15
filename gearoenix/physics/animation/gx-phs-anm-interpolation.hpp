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
    if constexpr (std::is_same_v<Value, math::Quat<double>> || std::is_same_v<Value, math::Quat<float>>) {
        // It can become better.
        auto dot = sk.dot(ek);
        if (dot > 0.999)
            dot = 0.999; // Preventing NaN in acos
        if (dot < -0.999)
            dot = -0.999; // Preventing NaN in acos
        const auto arc_cosine = std::acos(std::abs(dot));
        if (arc_cosine < 0.001)
            return (sk * (1.0 - t)) + (ek * t); // Preventing NaN in slerp
        const auto arc_cosine_t = arc_cosine * t;
        const auto e_sin = dot > 0.0 ? std::sin(arc_cosine_t) : -std::sin(arc_cosine_t);
        const auto dom = 1.0 / std::sin(arc_cosine);
        return (sk * (std::sin(arc_cosine - arc_cosine_t) * dom)) + (ek * (e_sin * dom));
    } else {
        return (sk * (1.0 - t)) + (ek * t);
    }
}
}

#endif