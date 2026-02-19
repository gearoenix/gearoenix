#pragma once
#include "../../math/gx-math-vector-3d.hpp"

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::camera {
struct ColourTuning final {
    typedef std::uint8_t index_t;

    struct GammaCorrection final {
        math::Vec3<float> gamma_exponent = math::Vec3(1.3f / 2.2f);

        void write(platform::stream::Stream& s) const;
        void read(platform::stream::Stream& s);
    };

    struct Multiply final {
        math::Vec3<float> scale = math::Vec3(1.0f);

        void write(platform::stream::Stream& s) const;
        void read(platform::stream::Stream& s);
    };

    struct Unchanged final { };

    constexpr static index_t gamma_correction_index = 0;
    constexpr static index_t multiply_index = 1;
    constexpr static index_t unchanged_index = 2;
    constexpr static index_t types_count = 3;

private:
    index_t index = gamma_correction_index;
    union {
        GammaCorrection gamma_correction;
        Multiply multiply;
        Unchanged unchanged;
    } data = {};

public:
    constexpr ColourTuning() { data.gamma_correction = {}; }

    constexpr ColourTuning(const ColourTuning&) = default;
    constexpr ColourTuning(ColourTuning&&) noexcept = default;
    constexpr ColourTuning& operator=(const ColourTuning&) = default;
    constexpr ColourTuning& operator=(ColourTuning&&) noexcept = default;

    constexpr ColourTuning& operator=(const GammaCorrection& v)
    {
        index = gamma_correction_index;
        data.gamma_correction = v;
        return *this;
    }

    constexpr ColourTuning& operator=(const Multiply& v)
    {
        index = multiply_index;
        data.multiply = v;
        return *this;
    }

    constexpr ColourTuning& operator=(const Unchanged& v)
    {
        index = unchanged_index;
        data.unchanged = v;
        return *this;
    }

    [[nodiscard]] constexpr index_t get_index() const { return index; }

    [[nodiscard]] constexpr bool is_gamma_correction() const { return index == gamma_correction_index; }

    [[nodiscard]] constexpr bool is_multiply() const { return index == multiply_index; }

    [[nodiscard]] constexpr bool is_unchanged() const { return index == unchanged_index; }

    [[nodiscard]] const GammaCorrection& get_gamma_correction() const;
    [[nodiscard]] const Multiply& get_multiply() const;
    [[nodiscard]] const Unchanged& get_unchanged() const;
    [[nodiscard]] bool show_debug_gui();
    void write(platform::stream::Stream& s) const;
    void read(platform::stream::Stream& s);
};
}