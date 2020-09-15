#ifndef GEAROENIX_GLC3_PIPELINE_RESOURCE_SET_HPP
#define GEAROENIX_GLC3_PIPELINE_RESOURCE_SET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/gx-cr-static.hpp"
#include "../../gl/gx-gl-types.hpp"
#include <memory>

#define GX_GLC3_PIP_RES_START_DRAWING_MESH base->bind(bound_shader_program);

#define GX_GLC3_PIP_RES_END_DRAWING_MESH                                       \
    reinterpret_cast<const buffer::Vertex*>(msh->get_vertex_buffer())->bind(); \
    reinterpret_cast<const buffer::Index*>(msh->get_index_buffer())->draw();   \
    gl::Loader::bind_vertex_array(0);

#define GX_GLC3_PIP_RES_START_SHADER(cls, shd) const shader::cls* shdr = reinterpret_cast<const shader::cls*>(base->get_shd().get());

#define GX_GLC3_PIP_RES_SET_UNIFORM(x, d) shdr->set_##x##_data(&(d));

#define GX_GLC3_PIP_RES_SET_TXT(x, txt, cls) static_cast<const texture::cls*>(txt)->bind(static_cast<gl::enumerated>(shdr->get_##x##_index()));

#define GX_GLC3_PIP_RES_SET_TXT_2D(x, txt) GX_GLC3_PIP_RES_SET_TXT(x, txt, Texture2D)

#define GX_GLC3_PIP_RES_SET_TXT_CUBE(x, txt) GX_GLC3_PIP_RES_SET_TXT(x, txt, TextureCube)

namespace gearoenix::glc3::shader {
class Shader;
}

namespace gearoenix::render::pipeline {
class ResourceSet;
}

namespace gearoenix::glc3::pipeline {
class ResourceSet {
    GX_GET_CREF_PRV(std::shared_ptr<shader::Shader>, shd)
public:
    explicit ResourceSet(std::shared_ptr<shader::Shader> shd) noexcept;
    virtual ~ResourceSet() noexcept;
    void bind(gl::uint& bound_shader_program) const noexcept;
    static void bind(const render::pipeline::ResourceSet* prs, gl::uint& bound_shader_program) noexcept;
};
}

#endif
#endif