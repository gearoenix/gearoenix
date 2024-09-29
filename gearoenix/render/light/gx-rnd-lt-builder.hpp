#ifndef GEAROENIX_RENDER_LIGHT_BUILDER_HPP
#define GEAROENIX_RENDER_LIGHT_BUILDER_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <array>

#ifdef far
#undef far
#endif

#ifdef near
#undef near
#endif

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::physics {
struct TransformationComponent;
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
    struct DirectionalInfo final {
    };

    struct ShadowCasterDirectionalInfo final {
        std::uint32_t shadow_map_resolution = 0;
        float far = 100.0f;
        float near = 1.0f;
        float aspect = 20.0f;
    };

    struct PointInfo final {
    };

    struct ShadowCasterPointInfo final {
    };

    struct SpotInfo final {
    };

    struct ShadowCasterSpotInfo final {
    };

    engine::Engine& e;
    std::array<std::shared_ptr<camera::Builder>, 6> camera_builders;
    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);

    Builder(
        engine::Engine& e,
        const std::string& name,
        physics::TransformationComponent* parent_transform,
        const DirectionalInfo& info,
        core::job::EndCaller<>&& end_callback);

    Builder(
        engine::Engine& e,
        const std::string& name,
        physics::TransformationComponent* parent_transform,
        const ShadowCasterDirectionalInfo& info,
        core::job::EndCaller<>&& end_callback);

public:
    Builder(const Builder&) = delete;
    Builder(Builder&&) = delete;
    virtual ~Builder();
    [[nodiscard]] const ShadowCasterDirectional* get_shadow_caster_directional() const;
    [[nodiscard]] ShadowCasterDirectional* get_shadow_caster_directional();
    [[nodiscard]] const Light& get_light() const;
    [[nodiscard]] Light& get_light();
    [[nodiscard]] core::ecs::entity_id_t get_id() const;
};
}
#endif