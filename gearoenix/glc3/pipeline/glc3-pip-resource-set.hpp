#ifndef GEAROENIX_GLC3_PIPELINE_RESOURCE_SET_HPP
#define GEAROENIX_GLC3_PIPELINE_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../gl/gl-types.hpp"
#include <memory>

#define GX_GLC3_PIP_RES_START_DRAWING_MESH glc3::pipeline::ResourceSet::bind(bound_shader_program);

#define GX_GLC3_PIP_RES_END_DRAWING_MESH                                       \
    reinterpret_cast<const buffer::Vertex*>(msh->get_vertex_buffer())->bind(); \
    reinterpret_cast<const buffer::Index*>(msh->get_index_buffer())->draw();   \
    gl::Loader::bind_vertex_array(0);

#define GX_GLC3_PIP_RES_START_SHADER(cls, shd) const shader::cls* shdr = reinterpret_cast<const shader::cls*>(shd.get());

#define GX_GLC3_PIP_RES_SET_UNIFORM(x, d) shdr->set_##x##_data(&(d));

#define GX_GLC3_PIP_RES_SET_TXT(x, txt, cls) reinterpret_cast<const texture::cls*>(txt)->bind(static_cast<gl::enumerated>(shdr->get_##x##_index()));

#define GX_GLC3_PIP_RES_SET_TXT_2D(x, txt) GX_GLC3_PIP_RES_SET_TXT(x, txt, Texture2D)

#define GX_GLC3_PIP_RES_SET_TXT_CUBE(x, txt) GX_GLC3_PIP_RES_SET_TXT(x, txt, Cube)

namespace gearoenix::glc3::shader {
class Shader;
}

namespace gearoenix::glc3::pipeline {
class ResourceSet {
protected:
    const std::shared_ptr<shader::Shader> shd;

    explicit ResourceSet(std::shared_ptr<shader::Shader> shd) noexcept;

public:
    virtual ~ResourceSet() noexcept;
    void bind(gl::uint& bound_shader_program) const noexcept;
};
}

#endif
#endif