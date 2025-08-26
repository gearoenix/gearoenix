#pragma once
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
struct RotateButton final : Button {
    const Button& previous;

    explicit RotateButton(const Button& previous);
    ~RotateButton() override;
    void update() override;
};
}