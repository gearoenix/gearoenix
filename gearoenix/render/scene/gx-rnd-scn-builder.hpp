#ifndef GEAROENIX_RENDER_SCENE_BUILDER_HPP
#define GEAROENIX_RENDER_SCENE_BUILDER_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>
#include <string>
#include <vector>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::render::camera {
struct Builder;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::model {
struct Builder;
}

namespace gearoenix::render::skybox {
struct Builder;
}

namespace gearoenix::render::scene {
struct Builder final {
    friend struct Manager;

    GX_GET_REFC_PRV(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder)

private:
    std::vector<std::shared_ptr<model::Builder>> model_builders;
    std::vector<std::shared_ptr<camera::Builder>> camera_builders;
    std::vector<std::shared_ptr<skybox::Builder>> skybox_builders;

    Builder(engine::Engine& e, const std::string& name, double layer) noexcept;

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    ~Builder() noexcept;

    void add(std::shared_ptr<model::Builder>&& model_builder) noexcept;
    void add(std::shared_ptr<camera::Builder>&& camera_builder) noexcept;
    void add(std::shared_ptr<skybox::Builder>&& skybox_builder) noexcept;
};
}

#endif