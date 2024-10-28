#ifndef GEAROENIX_EDITOR_VIEWPORT_ROTATE_BUTTON_HPP
#define GEAROENIX_EDITOR_VIEWPORT_ROTATE_BUTTON_HPP
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor {
struct Application;
}

namespace gearoenix::editor::viewport {
struct RotateButton final : Button {
    Application& app;
    const Button& previous;

    explicit RotateButton(Application& app, const Button& previous);
    ~RotateButton() override;
    void update() override;
};
}

#endif