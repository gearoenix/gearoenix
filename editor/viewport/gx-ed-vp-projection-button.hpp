#pragma once
#include "gx-ed-vp-button.hpp"
#include <gearoenix/render/camera/gx-rnd-cmr-projection.hpp>

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor {
struct Application;
}

namespace gearoenix::editor::viewport {
struct ProjectionButton final : Button {
    Application& app;
    render::camera::ProjectionData previous_projection_data = render::camera::ProjectionData::construct_orthographic();

    explicit ProjectionButton(Application& app);
    ~ProjectionButton() override;
    void update() override;
};
}