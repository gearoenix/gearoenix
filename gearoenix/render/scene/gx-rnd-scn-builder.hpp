#ifndef GEAROENIX_RENDER_SCENE_BUILDER_HPP
#define GEAROENIX_RENDER_SCENE_BUILDER_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <map>
#include <memory>
#include <string>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::render::camera {
struct Builder;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::light {
struct Builder;
}

namespace gearoenix::render::model {
struct Builder;
}

namespace gearoenix::render::reflection {
struct Builder;
}

namespace gearoenix::render::skybox {
struct Builder;
}

namespace gearoenix::render::scene {
struct Scene;
struct Builder final {
    friend struct Manager;

    typedef std::map<std::string, std::shared_ptr<model::Builder>> ModelBuildersMap;
    typedef std::map<std::string, std::shared_ptr<camera::Builder>> CameraBuildersMap;
    typedef std::map<std::string, std::shared_ptr<skybox::Builder>> SkyboxBuildersMap;
    typedef std::map<std::string, std::shared_ptr<reflection::Builder>> ReflectionBuildersMap;
    typedef std::map<std::string, std::shared_ptr<light::Builder>> LightBuildersMap;

    GX_GET_REFC_PRV(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);
    GX_GET_CREF_PRV(ModelBuildersMap, model_builders);
    GX_GET_CREF_PRV(CameraBuildersMap, camera_builders);
    GX_GET_CREF_PRV(SkyboxBuildersMap, skybox_builders);
    GX_GET_CREF_PRV(ReflectionBuildersMap, reflection_builders);
    GX_GET_CREF_PRV(LightBuildersMap, light_builders);

private:
    Builder(engine::Engine& e, const std::string& name, double layer, core::sync::EndCaller&& end_callback) noexcept;

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    ~Builder() noexcept;

    void add(std::shared_ptr<camera::Builder>&& camera_builder) noexcept;
    void add(std::shared_ptr<model::Builder>&& model_builder) noexcept;
    void add(std::shared_ptr<reflection::Builder>&& reflection_builder) noexcept;
    void add(std::shared_ptr<skybox::Builder>&& skybox_builder) noexcept;
    void add(std::shared_ptr<light::Builder>&& light_builder) noexcept;
    [[nodiscard]] Scene& get_scene() noexcept;
    [[nodiscard]] const Scene& get_scene() const noexcept;
    [[nodiscard]] core::ecs::entity_id_t get_id() const noexcept;
};
}

#endif