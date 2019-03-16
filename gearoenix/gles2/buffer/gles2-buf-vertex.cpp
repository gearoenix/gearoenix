#include "gles2-buf-vertex.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"

gearoenix::gles2::buffer::Vertex::Vertex(
	const std::vector<math::BasicVertex> vertices,
    const std::shared_ptr<engine::Engine>& e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
    : render::buffer::Static(static_cast<unsigned int>(vertices.size() * sizeof(math::BasicVertex)), e)
{
    e->get_function_loader()->load([this, vertices, c] {
		glGenBuffers(1, &bo);
		glBindBuffer(GL_ARRAY_BUFFER, bo);
		glBufferData(GL_ARRAY_BUFFER, size, vertices.data(), GL_STATIC_DRAW);
	});
}

gearoenix::gles2::buffer::Vertex::~Vertex()
{
    if (bo == 0)
        return;
	const GLuint cbo = bo;
    e->get_function_loader()->load([cbo] {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &cbo);
    });
    bo = 0;
}

#endif
