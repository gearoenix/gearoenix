#pragma once
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor {
struct Application;
}

namespace gearoenix::editor::viewport {
struct TranslateButton final : Button {
    Application& app;
    const Button& previous;

    explicit TranslateButton(Application& app, const Button& previous);
    ~TranslateButton() override;
    void update() override;
};
}