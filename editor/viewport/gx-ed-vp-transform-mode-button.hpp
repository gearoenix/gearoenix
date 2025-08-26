#pragma once
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
struct TransformModeButton final : Button {
    const Button& previous;

    explicit TransformModeButton(const Button& previous);
    ~TransformModeButton() override;
    void update() override;
};
}