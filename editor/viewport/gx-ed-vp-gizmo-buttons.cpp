#include "gx-ed-vp-gizmo-buttons.hpp"
#include "gx-ed-vp-rotate-button.hpp"
#include "gx-ed-vp-scale-button.hpp"
#include "gx-ed-vp-translate-button.hpp"

gearoenix::editor::viewport::GizmoButtons::GizmoButtons(Application& app, const Button& previous)
    : app(app)
    , previous(previous)
    , translate(new TranslateButton(app, previous))
    , rotate(new RotateButton(app, *translate))
    , scale(new ScaleButton(app, *rotate))
{
}

gearoenix::editor::viewport::GizmoButtons::~GizmoButtons() = default;

void gearoenix::editor::viewport::GizmoButtons::update()
{
    translate->update();
    rotate->update();
    scale->update();
}
