#ifndef GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#define GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#include "../../math/math-vector-3d.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "rnd-wdg-type.hpp"

namespace gearoenix::render::widget {
class Widget : public model::Model {
    GX_GET_CVAL_PRT(Type, widget_type)
    GX_GETSET_AVAL_PRT(bool, sensivity, true)
protected:
    Widget(
        core::Id my_id,
        Type t,
        system::stream::Stream* s,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Widget(
        core::Id my_id,
        Type t,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    ~Widget() noexcept override;
    static std::shared_ptr<Widget> read_gx3d(
        core::Id my_id,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    [[nodiscard]] bool get_dynamicity() const noexcept final;
    virtual void selected(const math::Vec3<double>& point) noexcept;
    virtual void selected_on(const math::Vec3<double>& point, const std::vector<model::Model*>& children) noexcept;
    virtual void select_cancelled() noexcept;
    virtual void select_released() noexcept;
    virtual void dragged(const math::Vec3<double>& point) noexcept;
    virtual void dragged_on(const math::Vec3<double>& point, const std::vector<model::Model*>& children) noexcept;
};
}
#endif
