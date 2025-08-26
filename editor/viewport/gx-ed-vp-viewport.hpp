#pragma once
#include <gearoenix/core/gx-cr-singleton.hpp>
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>

#include <memory>
#include <string>

namespace gearoenix::editor::viewport {
struct ProjectionButton;
struct GizmoButtons;
struct Camera;
struct Viewport final : core::Singleton<Viewport> {
    const std::string invisible_window_id;

    GX_GET_CREF_PRV(std::unique_ptr<ProjectionButton>, projection_button);
    GX_GET_CREF_PRV(std::unique_ptr<GizmoButtons>, gizmo_buttons);
    GX_GET_CREF_PRV(std::unique_ptr<Camera>, camera);

public:
    Viewport();
    ~Viewport() override;
    void update();
    void renew();
};
}