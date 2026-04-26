#pragma once
#include "gx-ed-vp-button.hpp"

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
/// Gizmo-toggle button that turns the translate handle on/off. Chains to the right of
/// `previous` on the toolbar row.
struct TranslateButton final : Button {
    const Button& previous;

    explicit TranslateButton(const Button& previous);
    ~TranslateButton() override;
    void update() override;

protected:
    [[nodiscard]] float compute_start_x() const override;
    [[nodiscard]] ImVec2 compute_size() const override;
};
}
