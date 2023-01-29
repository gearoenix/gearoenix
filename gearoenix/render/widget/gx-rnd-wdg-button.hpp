#ifndef GEAROENIX_RENDER_WIDGET_BUTTON_HPP
#define GEAROENIX_RENDER_WIDGET_BUTTON_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "gx-rnd-wdg-widget.hpp"

namespace gearoenix::render::model {
struct Builder;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::widget {
struct Button final : public Widget {
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, rest_texture);
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, pressed_texture);

private:
    void set_on_press_impl(const std::function<void(const math::Vec3<double>&)>& fun) noexcept;
    void set_on_release_impl(const std::function<void(const math::Vec3<double>&)>& fun) noexcept;
    void set_on_cancel_impl(const std::function<void()>& fun) noexcept;

public:
    Button(std::string&& name, engine::Engine& e) noexcept;
    ~Button() noexcept final;
    [[nodiscard]] static std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Button>> construct(
        std::string&& name,
        engine::Engine& e,
        const std::string& pressed_texture_asset,
        const std::string& rest_texture_asset,
        core::ecs::entity_id_t camera_id,
        Widget& parent,
        scene::Builder& scene_builder,
        const core::sync::EndCaller& end_callback) noexcept;
    void set_on_press(const std::function<void(const math::Vec3<double>&)>& fun) noexcept final;
    void set_on_release(const std::function<void(const math::Vec3<double>&)>& fun) noexcept final;
    void set_on_cancel(const std::function<void()>& fun) noexcept final;
};
}
#endif
