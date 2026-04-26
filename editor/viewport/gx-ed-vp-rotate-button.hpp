#pragma once
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
/// Gizmo-toggle button for the rotation handle. Chains to the right of `previous`.
struct RotateButton final : Button {
    const Button& previous;

    explicit RotateButton(const Button& previous);
    ~RotateButton() override;
    void update() override;

protected:
    [[nodiscard]] float compute_start_x() const override;
    [[nodiscard]] ImVec2 compute_size() const override;
};
}
