#include "gles2-buf-vertex.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"

gearoenix::gles2::buffer::Vertex::Vertex(
	const std::vector<math::BasicVertex> vertices,
    const std::shared_ptr<engine::Engine>& e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
    : render::buffer::Static(static_cast<unsigned int>(vertices.size() * sizeof(math::BasicVertex)), e)
{
    e->get_function_loader()->load([this, vertices, c] {
		gl::gen_buffers(1, &bo);
		gl::bind_buffer(GL_ARRAY_BUFFER, bo);
		gl::buffer_data(GL_ARRAY_BUFFER, size, vertices.data(), GL_STATIC_DRAW);
	});
}

gearoenix::gles2::buffer::Vertex::~Vertex()
{
    if (bo == 0)
        return;
	const gl::uint cbo = bo;
    e->get_function_loader()->load([cbo] {
        gl::bind_buffer(GL_ARRAY_BUFFER, 0);
        gl::delete_buffers(1, &cbo);
    });
    bo = 0;
}

#endif
