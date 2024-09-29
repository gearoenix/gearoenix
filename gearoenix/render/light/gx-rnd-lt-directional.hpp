#ifndef GEAROENIX_RENDER_LIGHT_DIRECTIONAL_HPP
#define GEAROENIX_RENDER_LIGHT_DIRECTIONAL_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-lt-light.hpp"

namespace gearoenix::physics {
struct TransformationComponent;
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
    static constexpr std::size_t MAX_COUNT = 16;

    GX_GET_CREF_PRT(math::Vec3<float>, direction);

    [[nodiscard]] const boost::container::flat_set<std::type_index>& get_all_the_hierarchy_types_except_component() const override;

public:
    explicit Directional(std::string&& name);
    [[nodiscard]] static std::shared_ptr<Directional> construct(std::string&& name);
    ~Directional() override;
};

struct ShadowCasterDirectional : Light {
    static constexpr std::size_t MAX_COUNT = 16;

    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, shadow_map);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, shadow_map_target);
    GX_GET_CREF_PRV(std::shared_ptr<camera::Camera>, shadow_camera);
    GX_GET_CREF_PRV(std::shared_ptr<physics::collider::Frustum>, shadow_frustum);
    GX_GET_CREF_PRV(std::shared_ptr<physics::TransformationComponent>, shadow_transform);
    GX_GET_VAL_PRT(core::ecs::entity_id_t, shadow_camera_entity_id, 0);

    ShadowCasterDirectional(std::type_index final_type_index, std::string&& name);

public:
    ~ShadowCasterDirectional() override;
    virtual void initialise(
        engine::Engine&,
        physics::TransformationComponent* parent_transform,
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

#endif
