#ifndef GEAROENIX_RENDER_WIDGET_EDIT_HPP
#define GEAROENIX_RENDER_WIDGET_EDIT_HPP
#include "rnd-wdg-text.hpp"

namespace gearoenix::render::font {
class Font2D;
}

namespace gearoenix::render::model {
class Dynamic;
}

namespace gearoenix::render::widget {
class Edit : public Text {
    GX_GET_CREF_PRV(std::shared_ptr<Text>, hint)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, background)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, cursor)
    GX_GET_VAL_PRV(std::size_t, capacity, 28);
    GX_GET_VAL_PRV(bool, cut_string_from_left, true)

    [[nodiscard]] std::wstring cut_string(const std::wstring& t) const noexcept;

public:
    Edit(
        core::Id my_id,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Edit(
        core::Id my_id,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Edit() noexcept final;
    void set_text(const std::wstring& t, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept final;
    void set_hint_text(const std::wstring& t, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    void set_capacity(unsigned int count) noexcept;
};
}
#endif
