#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-types.hpp"

namespace gearoenix::gl {
struct Texture2D;
}

namespace gearoenix::gl {
struct Camera;
struct Model;
struct Mesh;
struct Scene;
}

namespace gearoenix::render::record {
struct Camera;
struct CameraModel;
struct Model;
}

namespace gearoenix::gl::material {
struct Material {
    Material();
    virtual ~Material();
    virtual void shadow(
        const Mesh&, const render::record::Camera&, const render::record::CameraModel&, uint& current_shader);
    virtual void render_forward(
        const Scene&, const render::record::Camera&, const render::record::CameraModel&, const Mesh&, uint& current_shader);
    virtual void deferred_gbuffer_render(const Model&, const Mesh&, const Camera&, const Scene&, uint& current_shader);
};
}

#endif