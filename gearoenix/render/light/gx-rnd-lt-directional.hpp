#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-lt-light.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::collider {
struct Frustum;
}

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Target;
struct Texture2D;
}

namespace gearoenix::render::light {
struct Builder;
struct Directional final : Light {
    constexpr static std::uint32_t MAX_COUNT = 16;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 13;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES { Light::TYPE_INDEX };
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES { Light::TYPE_INDEX };

    GX_GET_CREF_PRT(math::Vec3<float>, direction);

public:
    Directional(std::string&& name, core::ecs::entity_id_t entity_id);
    ~Directional() override;
};

struct ShadowCasterDirectional : Light {
    constexpr static std::uint32_t MAX_COUNT = 16;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 11;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES { Light::TYPE_INDEX };
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES { Light::TYPE_INDEX };

    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, shadow_map);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, shadow_map_target);
    GX_GET_CREF_PRV(std::shared_ptr<camera::Camera>, shadow_camera);
    GX_GET_CREF_PRV(std::shared_ptr<physics::collider::Frustum>, shadow_frustum);
    GX_GET_CREF_PRV(std::shared_ptr<physics::Transformation>, shadow_transform);
    GX_GET_VAL_PRT(core::ecs::entity_id_t, shadow_camera_entity_id, core::ecs::invalid_entity_id);

    ShadowCasterDirectional(core::ecs::component_index_t final_type_index, std::string&& name, core::ecs::entity_id_t entity_id);

public:
    ~ShadowCasterDirectional() override;
    virtual void initialise(
        engine::Engine&,
        physics::Transformation* parent_transform,
        std::uint32_t resolution,
        float camera_far,
        float camera_near,
        float camera_aspect,
        const std::shared_ptr<Builder>& builder,
        core::job::EndCaller<>&& end_callback);
    virtual void initialise_camera(float camera_far, float camera_near, float camera_aspect);
    virtual void set_shadow_map(std::uint32_t resolution, core::job::EndCaller<>&& end_callback);
    virtual void set_shadow_map(std::shared_ptr<texture::Texture2D>&&, core::job::EndCaller<>&& end_callback);
    virtual void set_shadow_map_target(std::shared_ptr<texture::Target>&&);
};
}
