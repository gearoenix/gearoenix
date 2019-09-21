#include "gles2-buf-index.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"

gearoenix::gles2::buffer::Index::Index(
    std::vector<std::uint32_t> indices,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::buffer::Static(static_cast<unsigned int>(indices.size() * sizeof(gl::ushort)), e)
{
    std::vector<gl::ushort> data(indices.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        data[i] = static_cast<gl::ushort>(indices[i]);
    }
    count = static_cast<gl::sizei>(indices.size());
    e->get_function_loader()->load([this, data { move(data) }, c] {
        gl::Loader::gen_buffers(1, &bo);
        gl::Loader::bind_buffer(GL_ELEMENT_ARRAY_BUFFER, bo);
        gl::Loader::buffer_data(GL_ELEMENT_ARRAY_BUFFER, buffer_size, data.data(), GL_STATIC_DRAW);
    });
}

gearoenix::gles2::buffer::Index::~Index() noexcept
{
    if (bo == 0)
        return;
    const gl::uint cbo = bo;
    e->get_function_loader()->load([cbo] {
        gl::Loader::bind_buffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        gl::Loader::delete_buffers(1, &cbo);
    });
    bo = 0;
}

void gearoenix::gles2::buffer::Index::bind() const noexcept
{
    gl::Loader::bind_buffer(GL_ELEMENT_ARRAY_BUFFER, bo);
}

void gearoenix::gles2::buffer::Index::draw() const noexcept
{
    gl::Loader::draw_elements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);
}

#endif
