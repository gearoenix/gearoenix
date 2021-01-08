#ifndef GEAROENIX_RENDER_WIDGET_BUTTON_HPP
#define GEAROENIX_RENDER_WIDGET_BUTTON_HPP
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-wdg-widget.hpp"
#include <chrono>

namespace gearoenix::physics::animation {
struct Animation;
}

namespace gearoenix::render::material {
struct Unlit;
}

namespace gearoenix::render::widget {
struct Text;
struct TextTheme;
struct ButtonTheme {
    math::Vec4<double> background_color = math::Vec4(0.4, 0.37254902, 0.21372549, 1.0);
    math::Vec4<double> text_color = math::Vec4(0.1, 0.1, 0.1, 1.0);
    double text_scale = 0.9f;
    const TextTheme* text_theme = nullptr;
};
struct Button final : public Widget {
    GX_GET_CREF_PRV(std::shared_ptr<Text>, text)
    GX_GET_CREF_PRV(std::shared_ptr<material::Unlit>, background_material)
    GX_GET_CREF_PRV(ButtonTheme, theme)
    GX_GETSET_VAL_PRV(double, pressed_size, 0.75)
    GX_GETSET_VAL_PRV(double, animation_duration, 0.1)
    GX_GETSET_VAL_PRV(bool, multiple_pressable, false)
    GX_GET_CREF_PRV(std::weak_ptr<Button>, button_self)
protected:
    double scale_down_progress = 1.0;
    std::weak_ptr<physics::animation::Animation> animation;
    std::function<void()> on_click = [] {};
    std::function<void(bool)> on_press = [](bool) {};

    Button(
        core::Id id,
        std::string name,
        platform::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Button(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Button> construct(
        core::Id id,
        std::string name,
        platform::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    [[nodiscard]] static std::shared_ptr<Button> construct(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Button() noexcept final;
    void selected(const math::Vec3<double>& point) noexcept final;
    void select_cancelled() noexcept final;
    void select_released() noexcept final;
    void set_on_click(const std::function<void()>& on_click) noexcept;
    void set_on_press(const std::function<void(bool)>& on_press) noexcept;
    void set_text(const std::wstring& t, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
};
}
#endif
