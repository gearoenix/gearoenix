#include "gx-ed-vp-gizmo-buttons.hpp"
#include "gx-ed-vp-rotate-button.hpp"
#include "gx-ed-vp-scale-button.hpp"
#include "gx-ed-vp-transform-mode-button.hpp"
#include "gx-ed-vp-translate-button.hpp"

gearoenix::editor::viewport::GizmoButtons::GizmoButtons(const Button& previous)
    : previous(previous)
    , transform_mode(std::make_unique<TransformModeButton>(previous))
    , translate(std::make_unique<TranslateButton>(*transform_mode))
    , rotate(std::make_unique<RotateButton>(*translate))
    , scale(std::make_unique<ScaleButton>(*rotate))
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
