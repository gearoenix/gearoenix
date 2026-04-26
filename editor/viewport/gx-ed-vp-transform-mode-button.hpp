#pragma once
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
/// Toggle button between global-space and local-space transform gizmos. Chains to
/// the right of `previous` and is wider than the square gizmo toggles because it
/// shows a label.
struct TransformModeButton final : Button {
    const Button& previous;

    explicit TransformModeButton(const Button& previous);
    ~TransformModeButton() override;
    void update() override;

protected:
    [[nodiscard]] float compute_start_x() const override;
    [[nodiscard]] ImVec2 compute_size() const override;
};
}
