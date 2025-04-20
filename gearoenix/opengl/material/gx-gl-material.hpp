#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-types.hpp"

namespace gearoenix::gl {
struct Texture2D;
}

namespace gearoenix::gl::submission {
struct Camera;
struct Model;
struct Mesh;
struct Scene;
}

namespace gearoenix::gl::material {
struct Material {
    Material();
    virtual ~Material();
    virtual void shadow(const submission::Model& model, const submission::Mesh& mesh, const submission::Camera& camera, uint& current_shader);
    virtual void forward_render(const submission::Model& model, const submission::Mesh& mesh, const submission::Camera& camera, const submission::Scene& scene, uint& current_shader);
    virtual void deferred_gbuffer_render(const submission::Model& model, const submission::Mesh& mesh, const submission::Camera& camera, const submission::Scene& scene, uint& current_shader);
};
}

#endif