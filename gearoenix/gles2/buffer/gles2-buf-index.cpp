#include "gles2-buf-index.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../gl/gl-types.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"

gearoenix::gles2::buffer::Index::Index(
	const std::vector<std::uint32_t> indices,
	const std::shared_ptr<engine::Engine>& e,
	const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
	: render::buffer::Static(static_cast<unsigned int>(indices.size() * sizeof(gl::ushort)), e)
{
	std::vector<gl::ushort> idata(indices.size());
	for (size_t i = 0; i < indices.size(); ++i) 
	{
        idata[i] = static_cast<gl::ushort>(indices[i]);
	}
	count = static_cast<gl::sizei>(indices.size());
	e->get_function_loader()->load([this, idata, c] {
		gl::gen_buffers(1, &bo);
		gl::bind_buffer(GL_ELEMENT_ARRAY_BUFFER, bo);
		gl::buffer_data(GL_ELEMENT_ARRAY_BUFFER, size, idata.data(), GL_STATIC_DRAW);
	});
}

gearoenix::gles2::buffer::Index::~Index()
{
	if (bo == 0)
		return;
	const gl::uint cbo = bo;
	e->get_function_loader()->load([cbo] {
		gl::bind_buffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		gl::delete_buffers(1, &cbo);
	});
	bo = 0;
}

#endif
