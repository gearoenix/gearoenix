#ifndef GEAROENIX_PHYSICS_ANIMATION_KEYFRAME_HPP
#define GEAROENIX_PHYSICS_ANIMATION_KEYFRAME_HPP
#include "../../math/gx-math-quaternion.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include <variant>

namespace gearoenix::physics::animation {
template <typename Value>
struct KeyframeGltf2Bezier final {
    Value key;
    Value in;
    Value out;
};

template <typename Value>
struct KeyframeLinear final {
    Value key;
};

template <typename Value>
struct KeyframeStep final {
    Value key;
};

template <typename Value>
using Keyframe = std::variant<
    KeyframeStep<Value>,
    KeyframeLinear<Value>,
    KeyframeGltf2Bezier<Value>>;

template <typename Value>
[[nodiscard]] constexpr const Value& get_key(const Keyframe<Value>& keyframe)
{
    if (std::holds_alternative<KeyframeLinear<Value>>(keyframe))
        return std::get<KeyframeLinear<Value>>(keyframe).key;
    if (std::holds_alternative<KeyframeStep<Value>>(keyframe))
        return std::get<KeyframeStep<Value>>(keyframe).key;
    return std::get<KeyframeGltf2Bezier<Value>>(keyframe).key;
}
}

#endif