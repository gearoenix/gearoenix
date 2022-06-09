#ifndef GEAROENIX_RENDER_WIDGET_TEXT_HPP
#define GEAROENIX_RENDER_WIDGET_TEXT_HPP
#include "../../core/gx-cr-static.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-wdg-alignment.hpp"
#include "gx-rnd-wdg-widget.hpp"

namespace gearoenix::render::font {
struct Font2D;
}

namespace gearoenix::render::model {
struct Dynamic;
}

namespace gearoenix::render::material {
struct Unlit;
}

namespace gearoenix::render::widget {
struct Text : public Widget {
    GX_GET_CREF_PRT(std::wstring, text)
    GX_GET_CREF_PRT(std::shared_ptr<font::Font2D>, text_font)
    GX_GET_CREF_PRT(math::Vec4<double>, text_color)
    GX_GET_VAL_PRT(Alignment, v_align, Alignment::Center)
    GX_GET_VAL_PRT(Alignment, h_align, Alignment::Center)
    GX_GET_CREF_PRT(std::shared_ptr<model::Dynamic>, text_model)
    GX_GET_CREF_PRT(std::shared_ptr<material::Unlit>, text_mesh_material)
    GX_GET_CREF_PRT(std::weak_ptr<Text>, text_self)
private:
    void initialize(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    void update_alignment() noexcept;

    Text(
        core::Id id,
        std::string name,
        platform::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Text(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Text> construct(
        core::Id id,
        std::string name,
        platform::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    [[nodiscard]] static std::shared_ptr<Text> construct(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Text() noexcept override;
    void set_text(
        const std::wstring& t,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void set_text_color(
        double red, double green, double blue,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void set_text_color(
        const math::Vec4<double>& v,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    [[nodiscard]] double get_text_width() const noexcept;
    [[nodiscard]] double get_text_height() const noexcept;
};
}
#endif
