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
struct Font;
}

namespace gearoenix::render::material {
struct Unlit;
}

namespace gearoenix::render::model {
struct Builder;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::widget {
struct Text final : public Widget {
    GX_GETSET_CREF_PRT(std::wstring, text);
    GX_GETSET_CREF_PRT(std::shared_ptr<font::Font>, text_font);
    GX_GETSET_CREF_PRT(math::Vec4<double>, text_colour);
    GX_GETSET_VAL_PRT(Alignment, vertical_align, Alignment::Center);
    GX_GETSET_VAL_PRT(Alignment, horizontal_align, Alignment::Center);
    GX_GET_CREF_PRT(std::shared_ptr<material::Unlit>, text_material);
    GX_GET_CREF_PRT(std::weak_ptr<Text>, text_self);

private:
    Text(std::string&& name, engine::Engine& e, const core::sync::EndCaller& c) noexcept;

public:
    [[nodiscard]] static std::pair<std::shared_ptr<Text>, std::shared_ptr<model::Builder>> construct(
        std::string&& name,
        engine::Engine& e,
        core::ecs::entity_id_t camera_id,
        Widget* parent,
        scene::Builder& scene_builder,
        const core::sync::EndCaller& c) noexcept;
    ~Text() noexcept final;
    [[nodiscard]] math::Vec2<double> get_text_size() const noexcept;
    void update_text(const core::sync::EndCaller& c) noexcept;
};
}
#endif
