#include "gx-ed-vp-viewport.hpp"
#include "gx-ed-vp-camera.hpp"
#include "gx-ed-vp-gizmo-buttons.hpp"
#include "gx-ed-vp-projection-button.hpp"
#include <gearoenix/math/gx-math-imgui.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>

gearoenix::editor::viewport::Viewport::Viewport(Application& app)
    : app(app)
    , projection_button(new ProjectionButton(app))
    , gizmo_buttons(new GizmoButtons(app, *projection_button))
    , camera(new Camera(app))
{
}

gearoenix::editor::viewport::Viewport::~Viewport() = default;

void gearoenix::editor::viewport::Viewport::update()
{
    projection_button->update();
    gizmo_buttons->update();
    camera->update();
}
