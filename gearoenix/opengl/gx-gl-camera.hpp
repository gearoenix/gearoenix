#ifndef GEAROENIX_GL_CAMERA_HPP
#define GEAROENIX_GL_CAMERA_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/camera/gx-rnd-cmr-builder.hpp"
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "../render/camera/gx-rnd-cmr-manager.hpp"

namespace gearoenix::gl {
struct Engine;
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

    static constexpr std::size_t CUSTOMISED_VAR_INDEX = 0;
    static constexpr std::size_t DEFAULT_VAR_INDEX = 1;

    std::variant<Customised, Default> target = Default {};

    ~CameraTarget();
    [[nodiscard]] static CameraTarget construct(const render::camera::Target& target);
    [[nodiscard]] const Customised& get_customised() const;
    [[nodiscard]] const Default& get_default() const;
};

struct Camera final : render::camera::Camera {
    GX_GET_CREF_PRV(CameraTarget, gl_target);

    [[nodiscard]] const HierarchyTypes& get_hierarchy_types() const override;
    void set_customised_target(std::shared_ptr<render::texture::Target>&&) override;
    void update_target(core::job::EndCaller<>&& end) override;

public:
    Camera(Engine& e, const std::string& name, render::camera::Target&& target, core::ecs::entity_id_t entity_id);
    static void construct(Engine& e, const std::string& name, core::job::EndCallerShared<Camera>&& c, core::ecs::entity_id_t entity_id);
    ~Camera() override;
};

struct CameraBuilder final : render::camera::Builder {
    Engine& eng;

    CameraBuilder(
        Engine& e,
        const std::string& name,
        core::job::EndCaller<>&& entity_end_caller,
        physics::TransformationComponent* parent_transform);
    static void construct(
        Engine& e,
        const std::string& name,
        physics::TransformationComponent* parent_transform,
        core::job::EndCallerShared<Builder>&& builder_end_caller,
        core::job::EndCaller<>&& entity_end_caller);
    ~CameraBuilder() override;
};

struct CameraManager final : render::camera::Manager {
private:
    void build(
        const std::string& name,
        physics::TransformationComponent* parent_transform,
        core::job::EndCallerShared<render::camera::Builder>&& builder_end_caller,
        core::job::EndCaller<>&& entity_end_caller) override;
    void window_resized() override;

public:
    explicit CameraManager(Engine& e);
    ~CameraManager() override;
};
}

#endif
#endif
