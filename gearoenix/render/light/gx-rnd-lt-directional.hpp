#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
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

namespace gearoenix::render::texture {
struct Target;
struct Texture2D;
}

namespace gearoenix::render::light {
struct Directional final : Light {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = 16;
    constexpr static auto object_type_index = gearoenix_render_light_directional_type_index;
    constexpr static std::array all_parent_object_type_indices { Light::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Light::object_type_index };

    GX_GET_CREF_PRT(math::Vec3<float>, direction);

    Directional(core::ecs::Entity* entity, std::string&& name);

public:
    ~Directional() override;
};

struct ShadowCasterDirectional : Light {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = 16;
    constexpr static auto object_type_index = gearoenix_render_light_shadow_caster_directional_type_index;
    constexpr static std::array all_parent_object_type_indices { Light::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Light::object_type_index };

    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, shadow_map);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, shadow_map_target);
    GX_GET_CREF_PRV(std::shared_ptr<camera::Camera>, shadow_camera);
    GX_GET_CREF_PRV(std::shared_ptr<physics::collider::Frustum>, shadow_frustum);
    GX_GET_CREF_PRV(std::shared_ptr<physics::Transformation>, shadow_transform);
    GX_GET_CREF_PRT(core::ecs::EntityPtr, shadow_camera_entity);

    ShadowCasterDirectional(core::ecs::Entity* entity, core::object_type_index_t final_type_index, std::string&& name);

public:
    ~ShadowCasterDirectional() override;
    virtual void initialise(std::uint32_t resolution, float camera_far, float camera_near, float camera_aspect, core::job::EndCaller<>&& end_callback);
    virtual void initialise_camera(float camera_far, float camera_near, float camera_aspect);
    virtual void set_shadow_map(std::uint32_t resolution, core::job::EndCaller<>&& end_callback);
    virtual void set_shadow_map(std::shared_ptr<texture::Texture2D>&&, core::job::EndCaller<>&& end_callback);
    virtual void set_shadow_map_target(std::shared_ptr<texture::Target>&&);
};
}
