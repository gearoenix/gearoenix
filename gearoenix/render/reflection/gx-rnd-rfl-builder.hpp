#ifndef GEAROENIX_RENDER_REFLECTION_BUILDER_HPP
#define GEAROENIX_RENDER_REFLECTION_BUILDER_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../texture/gx-rnd-txt-format.hpp"
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

namespace gearoenix::render::texture {
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Runtime;
struct Builder {
    typedef std::array<std::shared_ptr<camera::Builder>, 6> CameraBuilders;

    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);
    GX_GET_CREF_PRT(CameraBuilders, faces_camera_builders);

public:
    /// Creates static reflection probe
    Builder(
        engine::Engine& e,
        const std::string& name,
        const math::Aabb3<double>& include_box,
        const std::shared_ptr<texture::TextureCube>& irradiance_texture,
        const std::shared_ptr<texture::TextureCube>& radiance_texture,
        const core::sync::EndCaller& end_callback) noexcept;
    /// Creates runtime reflection probe
    Builder(
        engine::Engine& e,
        const std::string& name,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::size_t environment_resolution,
        std::size_t irradiance_resolution,
        std::size_t radiance_resolution,
        const core::sync::EndCaller& end_callback) noexcept;
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    [[nodiscard]] const Runtime& get_runtime() const noexcept;
    [[nodiscard]] Runtime& get_runtime() noexcept;
    virtual ~Builder() noexcept;
    void set_camera_builder(std::shared_ptr<camera::Builder>&& builder, std::size_t face_index) noexcept;
};
}
#endif
