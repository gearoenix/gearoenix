#include "gles2-buf-index.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"

gearoenix::gles2::buffer::Index::Index(
	const std::vector<std::uint32_t> indices,
	const std::shared_ptr<engine::Engine>& e,
	const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
	: render::buffer::Static(static_cast<unsigned int>(indices.size() * sizeof(GLushort)), e)
{
	std::vector<GLushort> idata(indices.size());
	for (size_t i = 0; i < indices.size(); ++i) 
	{
		idata[i] = indices[i];
	}
	count = static_cast<GLsizei>(indices.size());
	e->get_function_loader()->load([this, idata, c] {
		glGenBuffers(1, &bo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, idata.data(), GL_STATIC_DRAW);
	});
}

gearoenix::gles2::buffer::Index::~Index()
{
	if (bo == 0)
		return;
	const GLuint cbo = bo;
	e->get_function_loader()->load([cbo] {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &cbo);
	});
	bo = 0;
}

#endif
