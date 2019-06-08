#ifndef GEAROENIX_GLES2_PIPELINE_RESOURCE_SET_HPP
#define GEAROENIX_GLES2_PIPELINE_RESOURCE_SET_HPP
#include "../../gl/gl-types.hpp"
#include <memory>

#define GX_GLES2_PIPRES_START_DRAWING_MESH                                     \
    reinterpret_cast<const buffer::Index*>(msh->get_index_buffer())->bind();   \
    reinterpret_cast<const buffer::Vertex*>(msh->get_vertex_buffer())->bind(); \
    gles2::pipeline::ResourceSet::bind(bound_shader_program)

#define GX_GLES2_PIPRES_END_DRAWING_MESH reinterpret_cast<const buffer::Index*>(msh->get_index_buffer())->draw()

#define GX_GLES2_PIPRES_START_SHADER(cls, shd) const shader::cls* shdr = static_cast<const shader::cls*>(shd.get())

#define GX_GLES2_PIPRES_SET_UNIFORM(x, d)           \
    if (shdr->get_##x() != GX_GLES2_UNIFORM_FAILED) \
    shdr->set_##x##_data(&(d))

#define GX_GLES2_PIPRES_SET_TXT(x, txt, cls)        \
    if (shdr->get_##x() != GX_GLES2_UNIFORM_FAILED) \
        static_cast<const texture::cls*>(txt)->bind(static_cast<gl::enumerated>(shdr->get_##x##_index()));

#define GX_GLES2_PIPRES_SET_TXT2D(x, txt) GX_GLES2_PIPRES_SET_TXT(x, txt, Texture2D)

#define GX_GLES2_PIPRES_SET_TXTCUBE(x, txt) GX_GLES2_PIPRES_SET_TXT(x, txt, Cube)

#define GX_GLES2_PIPRES_BIND_MATERIAL                                                                                                        \
    {                                                                                                                                        \
        const render::material::Uniform* material = reinterpret_cast<const render::material::Uniform*>(material_uniform_buffer->get_data()); \
        GX_GLES2_PIPRES_SET_UNIFORM(material_alpha, material->alpha);                                                                        \
        GX_GLES2_PIPRES_SET_UNIFORM(material_alpha_cutoff, material->alpha_cutoff);                                                          \
        GX_GLES2_PIPRES_SET_TXT2D(material_base_color, color);                                                                               \
        GX_GLES2_PIPRES_SET_TXT2D(material_emissive, emissive);                                                                              \
        GX_GLES2_PIPRES_SET_UNIFORM(material_metallic_factor, material->metallic_factor);                                                    \
        GX_GLES2_PIPRES_SET_TXT2D(material_metallic_roughness, metallic_roughness);                                                          \
        GX_GLES2_PIPRES_SET_TXT2D(material_normal, normal);                                                                                  \
        GX_GLES2_PIPRES_SET_UNIFORM(material_normal_scale, material->normal_scale);                                                          \
        GX_GLES2_PIPRES_SET_UNIFORM(material_occlusion_strength, material->occlusion_strength);                                              \
        GX_GLES2_PIPRES_SET_UNIFORM(material_roughness_factor, material->roughness_factor);                                                  \
    }

namespace gearoenix::gles2 {
namespace shader {
    class Shader;
}
namespace pipeline {
    class ResourceSet {
    protected:
        const std::shared_ptr<shader::Shader> shd;
        explicit ResourceSet(const std::shared_ptr<shader::Shader>& shd);

    public:
        virtual ~ResourceSet();
        virtual void bind(gl::uint& bound_shader_program) const;
    };
}
}
#endif
