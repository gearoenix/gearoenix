#ifndef GEAROENIX_GL_MATERIAL_HPP
#define GEAROENIX_GL_MATERIAL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-types.hpp"

namespace gearoenix::gl {
struct Engine;
struct Texture2D;
}

namespace gearoenix::gl::submission {
struct Camera;
struct Model;
struct Scene;
}

namespace gearoenix::gl::material {
struct Material {
    virtual ~Material() noexcept;
    virtual void shadow(const submission::Model& model, const submission::Camera& camera, uint& current_shader) noexcept;
    virtual void forward_render(const submission::Model& model, const submission::Camera& camera, const submission::Scene& scene, uint& current_shader) noexcept;
    virtual void deferred_gbuffer_render(const submission::Model& model, const submission::Camera& camera, const submission::Scene& scene, uint& current_shader) noexcept;
};
}

#endif
#endif