#ifndef GEAROENIX_RENDER_WIDGET_SIZE_HPP
#define GEAROENIX_RENDER_WIDGET_SIZE_HPP

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::widget {
struct Size final {
private:
    enum struct Type : unsigned char { HeightBased,
        WidthBased,
        PixelBased };
    Type type_index;
    double value;

    Size(const Type type_index, const double value) noexcept
        : type_index(type_index)
        , value(value)
    {
    }

public:
    Size(const Size&) noexcept = default;
    Size(Size&&) noexcept = default;
    Size& operator=(const Size&) noexcept = default;
    Size& operator=(Size&&) noexcept = default;
    [[nodiscard]] static Size width_based(const double value) noexcept { return { Type::WidthBased, value }; }
    [[nodiscard]] static Size height_based(const double value) noexcept { return { Type::HeightBased, value }; }
    [[nodiscard]] static Size pixel(const double value) noexcept { return { Type::PixelBased, value }; }
    [[nodiscard]] bool is_width_based() const noexcept { return Type::WidthBased == type_index; }
    [[nodiscard]] bool is_height_based() const noexcept { return Type::HeightBased == type_index; }
    [[nodiscard]] bool is_pixel() const noexcept { return Type::PixelBased == type_index; }
    [[nodiscard]] double to_world(const engine::Engine& e) const noexcept;
};
}

#endif