#include "gx-gl-material.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../platform/gx-plt-log.hpp"

gearoenix::gl::material::Material::Material(const bool needs_mvp)
    : needs_mvp(needs_mvp)
{
}

gearoenix::gl::material::Material::~Material() = default;

void gearoenix::gl::material::Material::shadow(
    const submission::Model&,
    const submission::Camera&,
    uint&)
{
    GX_UNIMPLEMENTED;
}

void gearoenix::gl::material::Material::forward_render(
    const submission::Model&,
    const submission::Camera&,
    const submission::Scene&,
    uint&)
{
    GX_UNIMPLEMENTED;
}

void gearoenix::gl::material::Material::deferred_gbuffer_render(
    const submission::Model&,
    const submission::Camera&,
    const submission::Scene&,
    uint&)
{
    GX_UNIMPLEMENTED;
}

#endif
