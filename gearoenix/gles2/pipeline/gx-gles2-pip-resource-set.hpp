#ifndef GEAROENIX_GLES2_PIPELINE_RESOURCE_SET_HPP
#define GEAROENIX_GLES2_PIPELINE_RESOURCE_SET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gx-gl-types.hpp"
#include "../../render/pipeline/gx-rnd-pip-resource-set.hpp"
#include <memory>

#define GX_GLES2_PIP_RES_START_DRAWING_MESH                                    \
    reinterpret_cast<const buffer::Index*>(msh->get_index_buffer())->bind();   \
    reinterpret_cast<const buffer::Vertex*>(msh->get_vertex_buffer())->bind(); \
    base->bind(bound_shader_program);

#define GX_GLES2_PIP_RES_END_DRAWING_MESH reinterpret_cast<const buffer::Index*>(msh->get_index_buffer())->draw();

#define GX_GLES2_PIP_RES_START_SHADER(cls, shd) const shader::cls* shdr = reinterpret_cast<const shader::cls*>(base->get_shd().get());

#define GX_GLES2_PIP_RES_SET_UNIFORM(x, d) shdr->set_##x##_data(&(d));

#define GX_GLES2_PIP_RES_SET_TXT(x, txt, cls) reinterpret_cast<const texture::cls*>(txt)->bind(static_cast<gl::enumerated>(shdr->get_##x##_index()));

#define GX_GLES2_PIP_RES_SET_TXT_2D(x, txt) GX_GLES2_PIP_RES_SET_TXT(x, txt, Texture2D)

#define GX_GLES2_PIP_RES_SET_TXT_CUBE(x, txt) GX_GLES2_PIP_RES_SET_TXT(x, txt, TextureCube)

namespace gearoenix::gles2::shader {
class Shader;
}

namespace gearoenix::gles2::pipeline {
class ResourceSet {
protected:
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