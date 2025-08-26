#pragma once
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
struct ScaleButton final : Button {
    const Button& previous;

    explicit ScaleButton(const Button& previous);
    ~ScaleButton() override;
    void update() override;
};
}