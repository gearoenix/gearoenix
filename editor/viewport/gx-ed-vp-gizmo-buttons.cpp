#include "gx-ed-vp-gizmo-buttons.hpp"
#include "gx-ed-vp-rotate-button.hpp"
#include "gx-ed-vp-scale-button.hpp"
#include "gx-ed-vp-transform-mode-button.hpp"
#include "gx-ed-vp-translate-button.hpp"

gearoenix::editor::viewport::GizmoButtons::GizmoButtons(Application& app, const Button& previous)
    : app(app)
    , previous(previous)
    , transform_mode(new TransformModeButton(app, previous))
    , translate(new TranslateButton(app, *transform_mode))
    , rotate(new RotateButton(app, *translate))
    , scale(new ScaleButton(app, *rotate))
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
