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

namespace gearoenix::render::texture {
struct Target;
}

namespace gearoenix::render::camera {
struct Manager;
struct Camera;
struct Builder {
    friend struct Manager;

    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder)

protected:
    Builder(engine::Engine& e, const std::string& name) noexcept;

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    virtual ~Builder() noexcept;

    [[nodiscard]] physics::Transformation& get_transformation() noexcept;
    [[nodiscard]] const physics::Transformation& get_transformation() const noexcept;
    [[nodiscard]] Camera& get_camera() noexcept;
    [[nodiscard]] const Camera& get_camera() const noexcept;
    // It is needed because the target must not be changed directly
    virtual void set_target(std::shared_ptr<texture::Target>&& target) noexcept;
};
}
#endif