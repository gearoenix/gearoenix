#pragma once
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
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

    typedef std::map<std::string, std::shared_ptr<model::Builder>> ModelBuildersMap;
    typedef std::map<std::string, std::shared_ptr<camera::Builder>> CameraBuildersMap;
    typedef std::map<std::string, std::shared_ptr<skybox::Builder>> SkyboxBuildersMap;
    typedef std::map<std::string, std::shared_ptr<reflection::Builder>> ReflectionBuildersMap;
    typedef std::map<std::string, std::shared_ptr<light::Builder>> LightBuildersMap;

    engine::Engine& e;
    GX_GET_REFC_PRV(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);
    GX_GET_CREF_PRV(ModelBuildersMap, model_builders);
    GX_GET_CREF_PRV(CameraBuildersMap, camera_builders);
    GX_GET_CREF_PRV(SkyboxBuildersMap, skybox_builders);
    GX_GET_CREF_PRV(ReflectionBuildersMap, reflection_builders);
    GX_GET_CREF_PRV(LightBuildersMap, light_builders);

public:
    Builder(engine::Engine& e, const std::string& name, double layer, core::job::EndCaller<>&& end_callback);
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    ~Builder();

    void add(std::shared_ptr<camera::Builder>&& camera_builder);
    void add(std::shared_ptr<model::Builder>&& model_builder);
    void add(std::shared_ptr<reflection::Builder>&& reflection_builder);
    void add(std::shared_ptr<skybox::Builder>&& skybox_builder);
    void add(std::shared_ptr<light::Builder>&& light_builder);
    [[nodiscard]] Scene& get_scene();
    [[nodiscard]] const Scene& get_scene() const;
    [[nodiscard]] core::ecs::entity_id_t get_id() const;
};
}