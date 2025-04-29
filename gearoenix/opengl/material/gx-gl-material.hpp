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

namespace gearoenix::render::record { struct Camera; struct CameraModel; struct Model; }

namespace gearoenix::gl::material {
struct Material {
    Material();
    virtual ~Material();
    virtual void shadow(
        const Mesh& mesh, const render::record::Camera&, const render::record::CameraModel&, uint& current_shader);
    virtual void render_forward(
        const Scene& scene, const render::record::Camera& camera, const render::record::CameraModel&,
        const Model& model, const Mesh& mesh, uint& current_shader);
    virtual void deferred_gbuffer_render(const Model& model, const Mesh& mesh, const Camera& camera, const Scene& scene, uint& current_shader);
};
}

#endif