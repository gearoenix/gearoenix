#include "gles2-buf-vertex.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"

gearoenix::gles2::buffer::Vertex::Vertex(
    const std::vector<math::BasicVertex>& vertices,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::buffer::Static(static_cast<unsigned int>(vertices.size() * sizeof(math::BasicVertex)), e)
{
    e->get_function_loader()->load([this, vertices, c] {
        gl::Loader::gen_buffers(1, &bo);
        gl::Loader::bind_buffer(GL_ARRAY_BUFFER, bo);
        gl::Loader::buffer_data(GL_ARRAY_BUFFER, buffer_size, vertices.data(), GL_STATIC_DRAW);
    });
}

gearoenix::gles2::buffer::Vertex::~Vertex() noexcept
{
    if (bo == 0)
        return;
    const gl::uint cbo = bo;
    e->get_function_loader()->load([cbo] {
        gl::Loader::bind_buffer(GL_ARRAY_BUFFER, 0);
        gl::Loader::delete_buffers(1, &cbo);
    });
    bo = 0;
}

void gearoenix::gles2::buffer::Vertex::bind() const noexcept
{
    gl::Loader::bind_buffer(GL_ARRAY_BUFFER, bo);
}

#endif
