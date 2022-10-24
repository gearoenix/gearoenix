#ifndef GEAROENIX_RENDER_LIGHT_BUILDER_HPP
#define GEAROENIX_RENDER_LIGHT_BUILDER_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <array>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::camera {
struct Builder;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::light {
struct Light;
struct Directional;
struct ShadowCasterDirectional;
struct Point;
struct Spot;
struct Builder {
    friend struct Manager;

    struct DirectionalInfo final {
    };

    struct ShadowCasterDirectionalInfo final {
        std::size_t shadow_map_resolution = 0;
        float far = 100.0;
        float near = 1.0;
        float aspect = 20;
    };

    struct PointInfo final {
    };

    struct ShadowCasterPointInfo final {
    };

    struct SpotInfo final {
    };

    struct ShadowCasterSpotInfo final {
    };

    typedef std::array<std::shared_ptr<camera::Builder>, 6> Cameras;

    engine::Engine& e;
    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);
    GX_GETSET_CREF_PRT(Cameras, cameras);

protected:
    Builder(
        engine::Engine& e,
        const std::string& name,
        const DirectionalInfo& info,
        const core::sync::EndCallerIgnored& end_callback) noexcept;

    Builder(
        engine::Engine& e,
        const std::string& name,
        const ShadowCasterDirectionalInfo& info,
        const core::sync::EndCallerIgnored& end_callback) noexcept;

public:
    Builder(const Builder&) = delete;
    Builder(Builder&&) = delete;
    virtual ~Builder() noexcept;
    [[nodiscard]] const ShadowCasterDirectional* get_shadow_caster_directional() const noexcept;
    [[nodiscard]] physics::Transformation& get_transformation(std::size_t camera_index = 0) noexcept;
    [[nodiscard]] Light& get_light() noexcept;
};
}
#endif