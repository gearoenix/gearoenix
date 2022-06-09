#ifndef GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#define GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#include "../../math/gx-math-vector-3d.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "gx-rnd-wdg-type.hpp"

namespace gearoenix::render::widget {
struct Widget : public model::Model {
    GX_GET_CVAL_PRT(Type, widget_type)
    GX_GET_CREF_PRT(std::weak_ptr<Widget>, widget_self)
    GX_GETSET_VAL_PRT(bool, sensivity, true)
protected:
    Widget(
        core::Id my_id,
        std::string name,
        Type t,
        platform::stream::Stream* s,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Widget(
        core::Id my_id,
        std::string name,
        Type t,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    ~Widget() noexcept override;
    static std::shared_ptr<Widget> read_gx3d(
        core::Id my_id,
        std::string name,
        platform::stream::Stream* f,
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
