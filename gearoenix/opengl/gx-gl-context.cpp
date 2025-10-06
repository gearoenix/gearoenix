#include "gx-gl-context.hpp"
#if GX_RENDER_OPENGL_ENABLED
#include "gx-gl-loader.hpp"
#include "gx-gl-constants.hpp"

void gearoenix::gl::ctx::set_framebuffer(const uint framebuffer_object)
{
    static uint current_bound_framebuffer = 0;

    if (current_bound_framebuffer == framebuffer_object) {
        return;
    }
    current_bound_framebuffer = framebuffer_object;
    if (0 == current_bound_framebuffer) {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, current_bound_framebuffer);
}

void gearoenix::gl::ctx::set_viewport_scissor_clip(const math::Vec4<sizei>& viewport_clip)
{
    static std::remove_cvref_t<decltype(viewport_clip)> current_viewport_clip {};

    if (current_viewport_clip == viewport_clip) {
        return;
    }
    current_viewport_clip = viewport_clip;
    glViewport(
        static_cast<sint>(current_viewport_clip.x), static_cast<sint>(current_viewport_clip.y),
        current_viewport_clip.z, current_viewport_clip.w);
    glScissor(
        static_cast<sint>(current_viewport_clip.x), static_cast<sint>(current_viewport_clip.y),
        current_viewport_clip.z, current_viewport_clip.w);
}
#endif