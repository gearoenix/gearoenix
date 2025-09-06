#pragma once
#include <gearoenix/core/ecs/gx-cr-ecs-entity-ptr.hpp>
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::constraint {
struct JetController;
}

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::editor::viewport {
struct Camera final {
    GX_GET_PTR_PRT(render::camera::Camera, camera);
    GX_GET_PTR_PRT(physics::Transformation, transformation);
    GX_GET_CREF_PRV(core::ecs::EntityPtr, controller_entity);
    GX_GET_CREF_PRV(std::shared_ptr<physics::constraint::JetController>, controller_component);

    void update_camera();
    void update_controller();
    void update_gizmo();

public:
    Camera();
    ~Camera();
    void update();
    [[nodiscard]] bool is_active() const;
};
}