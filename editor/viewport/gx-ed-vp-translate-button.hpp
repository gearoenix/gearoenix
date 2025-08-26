#pragma once
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
struct TranslateButton final : Button {
    const Button& previous;

    explicit TranslateButton(const Button& previous);
    ~TranslateButton() override;
    void update() override;
};
}