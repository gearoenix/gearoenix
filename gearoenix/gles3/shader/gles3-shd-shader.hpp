#ifndef GEAROENIX_GLES3_SHADER_SHADER_HPP
#define GEAROENIX_GLES3_SHADER_SHADER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../core/cr-static.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../gl/gl-types.hpp"
#include "../gles3.hpp"
#include <string>

#define GX_GLES3_UNIFORM_FAILED -1
#define GX_GLES3_TEXTURE_INDEX_FAILED -1

#ifdef GX_DEBUG_GLES3
#define GX_DEBUG_GLES3_GLSL
#endif

#define GX_GLES3_UNIFORM_TEXTURE(name)                     \
private:                                                   \
    gl::sint name = GX_GLES3_UNIFORM_FAILED;               \
    gl::sint name##_index = GX_GLES3_TEXTURE_INDEX_FAILED; \
                                                           \
public:                                                    \
    GX_GETTER_BUILDER(name)                                \
    GX_GETTER_BUILDER(name##_index)

#define GX_GLES3_UNIFORM(name, function)                     \
private:                                                     \
    gl::sint name = GX_GLES3_UNIFORM_FAILED;                 \
                                                             \
public:                                                      \
    GX_GETTER_BUILDER(name)                                  \
    void set_##name##_data(const float* data) const noexcept \
    {                                                        \
        gl::Loader::uniform##function;                       \
    }

#define GX_GLES3_UNIFORM_VECTOR(name, element_count, count) \
    GX_GLES3_UNIFORM(name, element_count##fv(name, count, data))

#define GX_GLES3_UNIFORM_FLOAT(name) \
    GX_GLES3_UNIFORM(name, 1f(name, *data))

#define GX_GLES3_UNIFORM_MATRIX(name, element_count, count) \
    GX_GLES3_UNIFORM(name, _matrix##element_count##fv(name, count, GL_FALSE, data))

#define GX_GLES3_GET_UNIFORM(shd, uniform) uniform = shd->get_uniform_location(#uniform);

#define GX_GLES3_GET_UNIFORM_F(shd, uniform)                 \
    GX_GLES3_GET_UNIFORM(shd, uniform);                      \
    if (GX_GLES3_UNIFORM_FAILED == uniform) {                \
        GXLOGF("Failed to locate the uniform " << #uniform); \
    }

#define GX_GLES3_THIS_GET_UNIFORM(uniform) GX_GLES3_GET_UNIFORM(this, uniform)

#define GX_GLES3_THIS_GET_UNIFORM_F(uniform) GX_GLES3_GET_UNIFORM_F(this, uniform)

#define GX_GLES3_SHADER_SET_TEXTURE_INDEX(x) \
    x##_index = texture_index;               \
    ++texture_index;

#define GX_GLES3_THIS_GET_UNIFORM_TEXTURE(uniform) \
	GX_GLES3_GET_UNIFORM(this, uniform)            \
	if(GX_GLES3_UNIFORM_FAILED != uniform)         \
	{                                              \
		GX_GLES3_SHADER_SET_TEXTURE_INDEX(uniform) \
	}

#define GX_GLES3_THIS_GET_UNIFORM_TEXTURE_F(uniform)\
	GX_GLES3_GET_UNIFORM_F(this, uniform)\
	GX_GLES3_SHADER_SET_TEXTURE_INDEX(uniform)

#define GX_GLES3_SHADER_SET_TEXTURE_INDEX_STARTING gl::sint texture_index = 0;

#define GX_GLES3_SHADER_SET_TEXTURE_INDEX_UNIFORM(x) \
    if (x != GX_GLES3_UNIFORM_FAILED)                \
        gl::Loader::uniform1i(x, x##_index);

#define GX_GLES3_SHADER_SRC_DEFAULT_PERCISION \
    "precision highp float;\n"                \
    "precision highp sampler2D;\n"            \
    "precision highp samplerCube;\n"          \
    "#define GXPI 3.14159265359\n"

#define GX_GLES3_SHADER_SRC_DEFAULT_ATTRIBUTES \
    "attribute vec3 position;\n"               \
    "attribute vec3 normal;\n"                 \
    "attribute vec4 tangent;\n"                \
    "attribute vec2 uv;\n"

#define GX_GLES3_SHADER_SRC_DEFAULT_VERTEX_STARTING GX_GLES3_SHADER_SRC_DEFAULT_PERCISION GX_GLES3_SHADER_SRC_DEFAULT_ATTRIBUTES

#define GX_GLES3_SHADER_SRC_DEFAULT_FRAGMENT_STARTING GX_GLES3_SHADER_SRC_DEFAULT_PERCISION

#define GX_GLES3_SHADER_SRC_MATERIAL_UNIFORMS          \
    "uniform float     material_alpha;\n"              \
    "uniform float     material_alpha_cutoff;\n"       \
    "uniform float     material_metallic_factor;\n"    \
    "uniform float     material_normal_scale;\n"       \
    "uniform float     material_occlusion_strength;\n" \
    "uniform float     material_roughness_factor;\n"

#define GX_GLES3_SHADER_SRC_MATERIAL_TEXTURES          \
    "uniform sampler2D material_base_color;\n"         \
    "uniform sampler2D material_metallic_roughness;\n" \
    "uniform sampler2D material_normal;\n"             \
    "uniform sampler2D material_emissive;\n"

#define GX_GLES3_SHADER_SRC_MATERIAL_RESOURCES GX_GLES3_SHADER_SRC_MATERIAL_UNIFORMS GX_GLES3_SHADER_SRC_MATERIAL_TEXTURES

#define GX_GLES3_SHADER_MATERIAL_UNIFORMS                 \
    GX_GLES3_UNIFORM_FLOAT(material_alpha)                \
    GX_GLES3_UNIFORM_FLOAT(material_alpha_cutoff)         \
    GX_GLES3_UNIFORM_TEXTURE(material_base_color)         \
    GX_GLES3_UNIFORM_TEXTURE(material_emissive)           \
    GX_GLES3_UNIFORM_FLOAT(material_metallic_factor)      \
    GX_GLES3_UNIFORM_TEXTURE(material_metallic_roughness) \
    GX_GLES3_UNIFORM_TEXTURE(material_normal)             \
    GX_GLES3_UNIFORM_FLOAT(material_normal_scale)         \
    GX_GLES3_UNIFORM_FLOAT(material_occlusion_strength)   \
    GX_GLES3_UNIFORM_FLOAT(material_roughness_factor)

#define GX_GLES3_SHADER_MATERIAL_GET_UNIFORM_LOCATIONS             \
    GX_GLES3_THIS_GET_UNIFORM(material_alpha)                      \
    GX_GLES3_THIS_GET_UNIFORM(material_alpha_cutoff)               \
    GX_GLES3_THIS_GET_UNIFORM_TEXTURE(material_base_color)         \
    GX_GLES3_THIS_GET_UNIFORM_TEXTURE(material_emissive)           \
    GX_GLES3_THIS_GET_UNIFORM(material_metallic_factor)            \
    GX_GLES3_THIS_GET_UNIFORM_TEXTURE(material_metallic_roughness) \
    GX_GLES3_THIS_GET_UNIFORM_TEXTURE(material_normal)             \
    GX_GLES3_THIS_GET_UNIFORM(material_normal_scale)               \
    GX_GLES3_THIS_GET_UNIFORM(material_occlusion_strength)         \
    GX_GLES3_THIS_GET_UNIFORM(material_roughness_factor)

#define GX_GLES3_SHADER_MATERIAL_SET_TEXTURE_INDEX_UNIFORM                 \
    GX_GLES3_SHADER_SET_TEXTURE_INDEX_UNIFORM(material_base_color)         \
    GX_GLES3_SHADER_SET_TEXTURE_INDEX_UNIFORM(material_emissive)           \
    GX_GLES3_SHADER_SET_TEXTURE_INDEX_UNIFORM(material_metallic_roughness) \
    GX_GLES3_SHADER_SET_TEXTURE_INDEX_UNIFORM(material_normal)

namespace gearoenix::gles3 {
namespace engine {
    class Engine;
}
namespace shader {
    class Shader {
    protected:
        const std::shared_ptr<engine::Engine> e;
        gl::uint shader_program = 0;
        gl::uint vertex_object = 0;
        gl::uint fragment_object = 0;
        gl::sint position_attribute_location = -1;
        gl::sint normal_attribute_location = -1;
        gl::sint tangent_attribute_location = -1;
        gl::sint uv_attribute_location = -1;
        void create_program();
        void run();
        void link();
        void validate();
        gl::uint add_shader_to_program(const std::string& shd, const gl::enumerated shader_type);
        gl::uint set_vertex_shader(const std::string& shd);
        gl::uint set_fragment_shader(const std::string& shd);
        static void end_program(const gl::uint shader_program);
        static void end_object(const gl::uint shader_object);

    public:
        Shader(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
        virtual ~Shader();
        /// On not found returns GX_SHADER_UNIFORM_FAILED
        gl::sint get_uniform_location(const std::string& name) const;
        gl::uint get_shader_program() const;
        virtual void bind() const;
    };
}
}
#endif
#endif
