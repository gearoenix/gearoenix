#ifndef GEAROENIX_RENDER_CAMERA_BUILDER_HPP
#define GEAROENIX_RENDER_CAMERA_BUILDER_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-rnd-cmr-projection.hpp"
#include <memory>
#include <string>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::camera {
struct Manager;
struct Builder final {
    friend struct Manager;

    GX_GET_REFC_PRV(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder)

private:
    Builder(engine::Engine& e, const std::string& name) noexcept;

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    ~Builder() noexcept;

    [[nodiscard]] physics::Transformation& get_transformation() noexcept;
    [[nodiscard]] const physics::Transformation& get_transformation() const noexcept;
    void set(Projection) noexcept;
};
}
#endif