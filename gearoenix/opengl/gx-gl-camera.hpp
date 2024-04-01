#ifndef GEAROENIX_GL_CAMERA_HPP
#define GEAROENIX_GL_CAMERA_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/camera/gx-rnd-cmr-builder.hpp"
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "../render/camera/gx-rnd-cmr-manager.hpp"

namespace gearoenix::render::camera {
struct BloomData;
}

namespace gearoenix::gl {
struct Engine;
struct Target;
struct Texture2D;

struct Camera final : render::camera::Camera {
    GX_GET_CREF_PRV(std::shared_ptr<Target>, gl_customised_target);

    [[nodiscard]] const boost::container::flat_set<std::type_index>& get_all_the_hierarchy_types_except_component() const override;
    void set_customised_target(std::shared_ptr<render::texture::Target>&&) override;

public:
    Camera(
        Engine& e,
        const std::string& name,
        std::size_t resolution_cfg_listener,
        std::shared_ptr<Target>&& customised_target,
        render::camera::ProjectionData projection_data,
        float near,
        float far,
        bool has_bloom);
    static std::shared_ptr<Camera> construct(
        Engine& e,
        const std::string& name,
        std::size_t resolution_cfg_listener,
        std::shared_ptr<Target>&& customised_target = nullptr,
        render::camera::ProjectionData projection_data = render::camera::PerspectiveProjectionData {},
        float near = 1.0f,
        float far = 100.0f,
        bool has_bloom = true);
    ~Camera() override;
};

struct CameraBuilder final : render::camera::Builder {
    Engine& eng;

    CameraBuilder(Engine& e, const std::string& name, core::job::EndCaller<>&& end_caller);
    ~CameraBuilder() override;
};

struct CameraManager final : render::camera::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::camera::Builder> build_v(
        const std::string& name, core::job::EndCaller<>&& end_caller) override;
    void window_resized() override;

public:
    explicit CameraManager(Engine& e);
    ~CameraManager() override;
};
}

#endif
#endif
