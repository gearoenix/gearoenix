#pragma once
#include "gx-phs-anm-keyframe.hpp"

namespace gearoenix::physics::animation {

enum struct Interpolation : std::uint8_t {
    Step = 11,
    Linear = 23,
    Gltf2CubicSPLine = 37,
};

template <typename Value, typename T>
[[nodiscard]] constexpr Value interpolate(const std::pair<core::fp_t, Keyframe<Value>>& start_keyframe, const std::pair<core::fp_t, Keyframe<Value>>& end_keyframe, const T time)
{
    const auto td = end_keyframe.first - start_keyframe.first;
    const auto t = (time - start_keyframe.first) / td;
    if (start_keyframe.second.is_gltf2_bezier() && end_keyframe.second.is_gltf2_bezier()) {
        const auto& s_key = start_keyframe.second.get_key();
        const auto& s_out = start_keyframe.second.get_out();
        const auto& e_in = end_keyframe.second.get_in();
        const auto& e_key = end_keyframe.second.get_key();
        const auto tp2 = t * t;
        const auto tp3 = tp2 * t;
        const auto tp2mp3 = tp2 * static_cast<core::fp_t>(3);
        return s_key * (tp3 * static_cast<core::fp_t>(2) - tp2mp3 + static_cast<core::fp_t>(1)) +
            s_out * (td * (tp3 - tp2 * static_cast<core::fp_t>(2) + t)) +
                e_key * (static_cast<core::fp_t>(-2) * tp3 + tp2mp3) + e_in * (td * (tp3 - tp2));
    }
    if (start_keyframe.second.is_step() || end_keyframe.second.is_step()) {
        if (t > static_cast<core::fp_t>(0.5)) {
            return end_keyframe.second.get_key();
        }
        return start_keyframe.second.get_key();
    }
    const auto& sk = start_keyframe.second.get_key();
    const auto& ek = end_keyframe.second.get_key();
    return sk.linear_mix(ek, t);
}
}