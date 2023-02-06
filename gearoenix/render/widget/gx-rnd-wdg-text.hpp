#ifndef GEAROENIX_RENDER_WIDGET_TEXT_HPP
#define GEAROENIX_RENDER_WIDGET_TEXT_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-wdg-alignment.hpp"
#include "gx-rnd-wdg-widget.hpp"

namespace gearoenix::core::sync {
struct EndCaller;
}

namespace gearoenix::render::font {
struct Font2D;
}

namespace gearoenix::render::material {
struct Unlit;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::widget {
struct Text final : public Widget {
    GX_GETSET_CREF_PRT(std::wstring, text);
    GX_GET_CREF_PRT(std::shared_ptr<font::Font2D>, text_font);
    GX_GETSET_CREF_PRT(math::Vec4<double>, text_colour);
    GX_GETSET_VAL_PRT(Alignment, vertical_align, Alignment::Center);
    GX_GETSET_VAL_PRT(Alignment, horizontal_align, Alignment::Center);
    GX_GET_VAL_PRT(core::ecs::entity_id_t, text_model, static_cast<core::ecs::entity_id_t>(-1));
    GX_GET_CREF_PRT(std::shared_ptr<material::Unlit>, text_material);
    GX_GET_CREF_PRT(std::weak_ptr<Text>, text_self);

private:
    Text(std::string&& name, engine::Engine& e, scene::Builder& scene_builder, const core::sync::EndCaller& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Text> construct(
        std::string&& name,
        engine::Engine& e,
        scene::Builder& scene_builder,
        const core::sync::EndCaller& c) noexcept;
    ~Text() noexcept final;
    [[nodiscard]] math::Vec2<double> get_text_size() const noexcept;
    void update_text(const core::sync::EndCaller& c) noexcept;
};
}
#endif
