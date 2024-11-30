#pragma once
#include <memory>

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor {
struct Application;
}

namespace gearoenix::editor::viewport {
struct Button;
struct TransformModeButton;
struct TranslateButton;
struct RotateButton;
struct ScaleButton;
struct GizmoButtons final {
    Application& app;
    const Button& previous;
    std::unique_ptr<TransformModeButton> transform_mode;
    std::unique_ptr<TranslateButton> translate;
    std::unique_ptr<RotateButton> rotate;
    std::unique_ptr<ScaleButton> scale;

    explicit GizmoButtons(Application& app, const Button& previous);
    ~GizmoButtons();
    void update();
};
}