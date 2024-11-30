#pragma once
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor {
struct Application;
}

namespace gearoenix::editor::viewport {
struct ScaleButton final : Button {
    Application& app;
    const Button& previous;

    explicit ScaleButton(Application& app, const Button& previous);
    ~ScaleButton() override;
    void update() override;
};
}