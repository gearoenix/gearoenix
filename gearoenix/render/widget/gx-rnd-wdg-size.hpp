#pragma once
#include "../../core/gx-cr-build-configuration.hpp"

namespace gearoenix::render::widget {
struct Size final {
private:
    enum struct Type : unsigned char {
        HeightBased,
        WidthBased,
        PixelBased
    };

    Type type_index;
    core::fp_t value;

    Size(const Type type_index, const core::fp_t value)
        : type_index(type_index)
        , value(value)
    {
    }

public:
    Size(const Size&) = default;
    Size(Size&&) = default;
    Size& operator=(const Size&) = default;
    Size& operator=(Size&&) = default;
    [[nodiscard]] static Size width_based(const core::fp_t value) { return { Type::WidthBased, value }; }
    [[nodiscard]] static Size height_based(const core::fp_t value) { return { Type::HeightBased, value }; }
    [[nodiscard]] static Size pixel(const core::fp_t value) { return { Type::PixelBased, value }; }
    [[nodiscard]] bool is_width_based() const { return Type::WidthBased == type_index; }
    [[nodiscard]] bool is_height_based() const { return Type::HeightBased == type_index; }
    [[nodiscard]] bool is_pixel() const { return Type::PixelBased == type_index; }
    [[nodiscard]] core::fp_t to_world() const;
};
}