#include "gx-gl-material.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../platform/gx-plt-log.hpp"

gearoenix::gl::material::Material::~Material() noexcept = default;

void gearoenix::gl::material::Material::shadow(
    const submission::Model&,
    const submission::Camera&,
    uint&) noexcept
{
    GX_UNIMPLEMENTED;
}

void gearoenix::gl::material::Material::forward_render(
    const submission::Model&,
    const submission::Camera&,
    const submission::Scene&,
    uint&) noexcept
{
    GX_UNIMPLEMENTED;
}

void gearoenix::gl::material::Material::deferred_gbuffer_render(
    const submission::Model&,
    const submission::Camera&,
    const submission::Scene&,
    uint&) noexcept
{
    GX_UNIMPLEMENTED;
}

#endif
