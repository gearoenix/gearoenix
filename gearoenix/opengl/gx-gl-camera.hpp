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

struct BloomData final {
    typedef std::array<std::shared_ptr<Target>, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT> Targets;
    typedef std::array<std::shared_ptr<Target>, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT + 1> UpTargets;

    GX_GET_CREF_PRV(std::shared_ptr<Target>, prefilter_target);
    GX_GET_CREF_PRV(Targets, horizontal_targets);
    GX_GET_CREF_PRV(Targets, vertical_targets);
    GX_GET_CREF_PRV(UpTargets, upsampler_targets);

    BloomData(
        std::shared_ptr<Target>&& prefilter_target,
        Targets&& horizontal_targets,
        Targets&& vertical_targets,
        UpTargets&& upsampler_targets);

public:
    ~BloomData();
    [[nodiscard]] static std::optional<BloomData> construct(const std::optional<render::camera::BloomData>& bloom_data);
};

struct CameraTarget final {
    struct Customised final {
        std::shared_ptr<Target> target;
    };

    struct Default final {
        std::shared_ptr<Target> first;
        std::shared_ptr<Target> second;
    };

    static constexpr std::size_t CUSTOMISED_VAR_INDEX = 0;
    static constexpr std::size_t DEFAULT_VAR_INDEX = 1;

    std::variant<Customised, Default> target = Default {};

    ~CameraTarget();
    [[nodiscard]] static CameraTarget construct(const render::camera::Target& target) noexcept;
    [[nodiscard]] const Customised& get_customised() const;
    [[nodiscard]] const Default& get_default() const;
};

struct Camera final : render::camera::Camera {
    GX_GET_CREF_PRV(CameraTarget, gl_target);
    GX_GET_CREF_PRV(std::optional<BloomData>, gl_bloom_data);

    [[nodiscard]] const boost::container::flat_set<std::type_index>& get_all_the_hierarchy_types_except_component() const override;
    void set_customised_target(std::shared_ptr<render::texture::Target>&&) override;
    void disable_bloom() override;
    void enable_bloom(core::job::EndCaller<>&& end) override;
    void update_target(core::job::EndCaller<>&& end) override;

public:
    Camera(Engine& e, const std::string& name, render::camera::Target&& target);
    static void construct(Engine& e, const std::string& name, core::job::EndCallerShared<Camera>&& c);
    ~Camera() override;
};

struct CameraBuilder final : render::camera::Builder {
    Engine& eng;

    CameraBuilder(Engine& e, const std::string& name, core::job::EndCaller<>&& entity_end_caller);
    static void construct(
        Engine& e,
        const std::string& name,
        core::job::EndCallerShared<render::camera::Builder>&& builder_end_caller,
        core::job::EndCaller<>&& entity_end_caller);
    ~CameraBuilder() override;
};

struct CameraManager final : render::camera::Manager {
private:
    void build(
        const std::string& name,
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
