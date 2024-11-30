#pragma once
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor {
struct Application;
}

namespace gearoenix::editor::viewport {
struct TransformModeButton final : Button {
    Application& app;
    const Button& previous;

    TransformModeButton(Application& app, const Button& previous);
    ~TransformModeButton() override;
    void update() override;
};
}