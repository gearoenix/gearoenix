#pragma once
#include "../../math/gx-math-quaternion.hpp"
#include "../../math/gx-math-vector-4d.hpp"

namespace gearoenix::physics::animation {
template <typename Value>
struct Keyframe final {
    constexpr static std::uint8_t step_type_index = 1;
    constexpr static std::uint8_t linear_type_index = 2;
    constexpr static std::uint8_t gltf2_bezier_type_index = 3;

    GX_GET_CREF_PRV(Value, key); // All the key-frame types have this.
    Value in;
    Value out;
    GX_GET_VAL_PRV(std::uint8_t, type, 0);

public:
    constexpr Keyframe() = default;
    constexpr Keyframe(Keyframe&&) = default;
    constexpr Keyframe(const Keyframe&) = default;
    constexpr Keyframe& operator=(Keyframe&&) = default;
    constexpr Keyframe& operator=(const Keyframe&) = default;

    [[nodiscard]] constexpr static Keyframe construct_step(const Value& key)
    {
        Keyframe k;
        k.key = key;
        k.type = step_type_index;
        return k;
    }

    [[nodiscard]] constexpr static Keyframe construct_linear(const Value& key)
    {
        Keyframe k;
        k.key = key;
        k.type = linear_type_index;
        return k;
    }

    [[nodiscard]] constexpr static Keyframe construct_gltf2_bezier(const Value& in, const Value& key, const Value& out)
    {
        Keyframe k;
        k.in = in;
        k.key = key;
        k.out = out;
        k.type = gltf2_bezier_type_index;
        return k;
    }

    [[nodiscard]] constexpr bool is_step() const { return type == step_type_index; }

    [[nodiscard]] constexpr bool is_linear() const { return type == linear_type_index; }

    [[nodiscard]] constexpr bool is_gltf2_bezier() const { return type == gltf2_bezier_type_index; }

    [[nodiscard]] const Value& get_in() const
    {
        GX_ASSERT_D(type == gltf2_bezier_type_index);
        return in;
    }

    [[nodiscard]] const Value& get_out() const
    {
        GX_ASSERT_D(type == gltf2_bezier_type_index);
        return out;
    }

    void write(platform::stream::Stream& s) const
    {
        s.write_fail_debug(type);
        switch (type) {
        case step_type_index:
        case linear_type_index:
            key.write(s);
            break;
        case gltf2_bezier_type_index:
            key.write(s);
            in.write(s);
            out.write(s);
            break;
        default:
            GX_UNEXPECTED;
        }
    }

    void read(platform::stream::Stream& s)
    {
        s.read(type);
        switch (type) {
        case step_type_index:
        case linear_type_index:
            key.read(s);
            break;
        case gltf2_bezier_type_index:
            key.read(s);
            in.read(s);
            out.read(s);
            break;
        default:
            GX_UNEXPECTED;
        }
    }
};
}