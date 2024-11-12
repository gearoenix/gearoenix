#ifndef GEAROENIX_RENDER_CAMERA_BUILDER_HPP
#define GEAROENIX_RENDER_CAMERA_BUILDER_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>
#include <string>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::physics {
struct TransformationComponent;
}

namespace gearoenix::physics::collider {
struct Frustum;
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

    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);

    Builder(
        engine::Engine& e, const std::string& name, core::job::EndCaller<>&& entity_in_world_callback,
        physics::TransformationComponent* parent_transform);

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    virtual ~Builder();

    [[nodiscard]] physics::TransformationComponent& get_transformation();
    [[nodiscard]] const physics::TransformationComponent& get_transformation() const;
    [[nodiscard]] std::shared_ptr<physics::TransformationComponent> get_transformation_shared_ptr() const;
    [[nodiscard]] physics::collider::Frustum& get_frustum();
    [[nodiscard]] const physics::collider::Frustum& get_frustum() const;
    [[nodiscard]] Camera& get_camera();
    [[nodiscard]] const Camera& get_camera() const;
    [[nodiscard]] core::ecs::entity_id_t get_id() const;
};
}
#endif