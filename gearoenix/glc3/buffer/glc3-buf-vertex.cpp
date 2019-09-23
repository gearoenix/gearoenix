#include "glc3-buf-vertex.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/glc3-eng-engine.hpp"

gearoenix::glc3::buffer::Vertex::Vertex(
    std::vector<math::BasicVertex> vertices,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::buffer::Static(0, e)
{
    const auto bf_size = static_cast<unsigned int>(vertices.size() * sizeof(math::BasicVertex));
    buffer_size = static_cast<std::size_t>(bf_size);
    e->get_function_loader()->load([this, vertices { move(vertices) }, c, bf_size] {
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
        gl::uint tvo = 0, tbo = 0;
        gl::Loader::gen_vertex_arrays(1, &tvo);
        gl::Loader::bind_vertex_array(tvo);
        gl::Loader::gen_buffers(1, &tbo);
        gl::Loader::bind_buffer(GL_ARRAY_BUFFER, tbo);
        gl::Loader::buffer_data(GL_ARRAY_BUFFER, static_cast<gl::sizeiptr>(bf_size), vertices.data(), GL_STATIC_DRAW);
        gl::Loader::enable_vertex_attrib_array(0);
        gl::Loader::enable_vertex_attrib_array(1);
        gl::Loader::enable_vertex_attrib_array(2);
        gl::Loader::enable_vertex_attrib_array(3);
        gl::Loader::vertex_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(math::BasicVertex), reinterpret_cast<void*>(0));
        gl::Loader::vertex_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(math::BasicVertex), reinterpret_cast<void*>(3 * sizeof(float)));
        gl::Loader::vertex_attrib_pointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(math::BasicVertex), reinterpret_cast<void*>(6 * sizeof(float)));
        gl::Loader::vertex_attrib_pointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(math::BasicVertex), reinterpret_cast<void*>(10 * sizeof(float)));
        gl::Loader::bind_vertex_array(0);
        vo = tvo;
        bo = tbo;
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
    });
}

gearoenix::glc3::buffer::Vertex::~Vertex() noexcept
{
    if (bo == 0)
        return;
    const gl::uint cbo = bo;
    const gl::uint cvo = vo;
    e->get_function_loader()->load([cbo, cvo] {
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
        gl::Loader::bind_vertex_array(0);
        gl::Loader::bind_buffer(GL_ARRAY_BUFFER, 0);
        gl::Loader::delete_vertex_arrays(1, &cvo);
        gl::Loader::delete_buffers(1, &cbo);
#ifdef GX_DEBUG_GL_CLASS_3
        gl::Loader::check_for_error();
#endif
    });
    bo = 0;
    vo = 0;
}

void gearoenix::glc3::buffer::Vertex::bind() const noexcept
{
    gl::Loader::bind_vertex_array(vo);
}

#endif
