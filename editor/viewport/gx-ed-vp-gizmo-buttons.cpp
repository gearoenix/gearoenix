#include "gx-ed-vp-gizmo-buttons.hpp"
#include "gx-ed-vp-rotate-button.hpp"
#include "gx-ed-vp-scale-button.hpp"
#include "gx-ed-vp-transform-mode-button.hpp"
#include "gx-ed-vp-translate-button.hpp"

gearoenix::editor::viewport::GizmoButtons::GizmoButtons(const Button& previous)
    : previous(previous)
    , transform_mode(new TransformModeButton(previous))
    , translate(new TranslateButton(*transform_mode))
    , rotate(new RotateButton(*translate))
    , scale(new ScaleButton(*rotate))
{
}

gearoenix::editor::viewport::GizmoButtons::~GizmoButtons() = default;

void gearoenix::editor::viewport::GizmoButtons::update()
{
    transform_mode->update();
    translate->update();
    rotate->update();
    scale->update();
}
