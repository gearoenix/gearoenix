#include "gx-gl-material.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../platform/gx-plt-log.hpp"

gearoenix::gl::material::Material::Material() = default;

gearoenix::gl::material::Material::~Material() = default;

void gearoenix::gl::material::Material::shadow(const Mesh&, const render::record::Camera&, const render::record::CameraModel&, uint&) { GX_UNIMPLEMENTED; }

void gearoenix::gl::material::Material::render_forward(const Scene&, const render::record::Camera&, const render::record::CameraModel&, const Mesh&, uint&) { GX_UNIMPLEMENTED; }

void gearoenix::gl::material::Material::deferred_gbuffer_render(const Model&, const Mesh&, const Camera&, const Scene&, uint&) { GX_UNIMPLEMENTED; }

#endif