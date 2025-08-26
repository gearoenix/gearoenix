#pragma once
#include "gx-ed-vp-button.hpp"

#include <gearoenix/render/camera/gx-rnd-cmr-projection.hpp>

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
struct ProjectionButton final : Button {
    render::camera::ProjectionData previous_projection_data = render::camera::ProjectionData::construct_orthographic();

    ProjectionButton();
    ~ProjectionButton() override;
    void update() override;
};
}