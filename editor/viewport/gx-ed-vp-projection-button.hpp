#pragma once
#include "gx-ed-vp-button.hpp"

#include <gearoenix/render/camera/gx-rnd-cmr-projection.hpp>

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
/// The first button on the viewport toolbar row: toggles the active camera between
/// perspective and orthographic projection, remembering the previous projection data,
/// so toggling back restores the original FOV/ortho-box rather than reverting to a
/// generic default.
struct ProjectionButton final : Button {
    render::camera::ProjectionData previous_projection_data = render::camera::ProjectionData::construct_orthographic();

    ProjectionButton();
    ~ProjectionButton() override;
    void update() override;

protected:
    [[nodiscard]] float compute_start_x() const override;
    [[nodiscard]] ImVec2 compute_size() const override;
};
}
