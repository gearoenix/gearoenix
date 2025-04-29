#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "../render/camera/gx-rnd-cmr-manager.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::render::record {  struct Camera; }

namespace gearoenix::gl::shader {
    struct SkyboxCube;
    struct SkyboxEquirectangular;
}

namespace gearoenix::gl {
    struct Scene;
struct Target;
struct Texture2D;

struct CameraTarget final {
    struct Customised final {
        std::shared_ptr<Target> target;
        // we can cache some in here and in default later if it can improve performance
    };

    struct Default final {
        std::shared_ptr<Target> main;
        std::array<std::array<std::shared_ptr<Target>, GX_RENDER_DEFAULT_CAMERA_TARGET_MIPS_COUNT>, 2> targets;
    };

    constexpr static std::uint32_t customised_var_index = 0;
    constexpr static std::uint32_t default_var_index = 1;

    std::variant<Customised, Default> target = Default {};

    ~CameraTarget();
    [[nodiscard]] static CameraTarget construct(const render::camera::Target& target);
    [[nodiscard]] const Customised& get_customised() const;
    [[nodiscard]] const Default& get_default() const;
};

struct Camera final : render::camera::Camera {
    constexpr static auto object_type_index = gearoenix_gl_camera_type_index;
    constexpr static std::array all_parent_object_type_indices { render::camera::Camera::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::camera::Camera::object_type_index };

    GX_GET_CREF_PRV(CameraTarget, gl_target);
    GX_GET_CREF_PRV(std::shared_ptr<shader::SkyboxCube>, skybox_cube_shader);
    GX_GET_CREF_PRV(std::shared_ptr<shader::SkyboxEquirectangular>, skybox_equirectangular_shader);

    void set_customised_target(std::shared_ptr<render::texture::Target>&&) override;
    void update_target(core::job::EndCaller<>&& end) override;

public:
    Camera(const std::string& name, render::camera::Target&& target, std::shared_ptr<physics::Transformation>&& transform);
    static void construct(const std::string& name, core::job::EndCallerShared<Camera>&& c, std::shared_ptr<physics::Transformation>&& transform);
    ~Camera() override;
    void render_shadow(const render::record::Camera&, uint& current_shader);
    void render_forward(const Scene& scene, const render::record::Camera&, uint& current_shader);
    void render_forward_skyboxes(const Scene& scene, const render::record::Camera&, uint& current_shader);
};

struct CameraManager final : render::camera::Manager {
private:
    void build(
        std::string&& name,
        core::ecs::Entity* parent,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;
    void window_resized() override;

public:
    explicit CameraManager();
    ~CameraManager() override;
};
}

#endif
