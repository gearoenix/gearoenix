#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-wdg-widget.hpp"

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::widget {
struct Button final : Widget {
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, rest_texture);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, pressed_texture);

    void set_on_press_impl(const std::function<void(const math::Vec3<double>&)>& fun);
    void set_on_release_impl(const std::function<void(const math::Vec3<double>&)>& fun);
    void set_on_cancel_impl(const std::function<void()>& fun);

public:
    Button(std::string&& name);
    ~Button() override;
    static void construct(
        std::string&& name,
        std::string&& pressed_texture_asset,
        std::string&& rest_texture_asset,
        core::ecs::Entity* camera_entity,
        std::shared_ptr<Widget>&& parent,
        core::job::EndCallerShared<Button>&& end_callback);
    static void construct(
        std::string&& name,
        std::shared_ptr<material::Material>&& mat,
        std::string&& pressed_texture_asset,
        std::string&& rest_texture_asset,
        core::ecs::Entity* camera_entity,
        std::shared_ptr<Widget>&& parent,
        core::job::EndCallerShared<Button>&& end_callback);
    static void construct(
        std::string&& name,
        std::shared_ptr<material::Material>&& mat,
        std::shared_ptr<texture::Texture2D>&& pressed_texture,
        std::string&& rest_texture_asset,
        core::ecs::Entity* camera_entity,
        std::shared_ptr<Widget>&& parent,
        core::job::EndCallerShared<Button>&& end_callback);
    static void construct(
        std::string&& name,
        std::shared_ptr<material::Material>&& mat,
        std::shared_ptr<texture::Texture2D>&& pressed_texture,
        std::shared_ptr<texture::Texture2D>&& rest_texture,
        core::ecs::Entity* camera_entity,
        std::shared_ptr<Widget>&& parent,
        core::job::EndCallerShared<Button>&& end_callback);
    static void construct(
        std::string&& name,
        std::shared_ptr<material::Material>&& mat,
        std::shared_ptr<texture::Texture2D>&& pressed_texture,
        std::shared_ptr<texture::Texture2D>&& rest_texture,
        std::shared_ptr<mesh::Mesh>&& button_mesh,
        core::ecs::Entity* camera_entity,
        std::shared_ptr<Widget>&& parent,
        core::job::EndCallerShared<Button>&& end_callback);
    void set_on_press(const std::function<void(const math::Vec3<double>&)>& fun) override;
    void set_on_release(const std::function<void(const math::Vec3<double>&)>& fun) override;
    void set_on_cancel(const std::function<void()>& fun) override;
    void set_rest_texture(std::shared_ptr<texture::Texture2D>&& rest_texture);
    void set_pressed_texture(std::shared_ptr<texture::Texture2D>&& pressed_texture);
};
}