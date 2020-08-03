#ifndef GEAROENIX_GLC3_SHADER_SHADER_HPP
#define GEAROENIX_GLC3_SHADER_SHADER_HPP

#include "../../core/cr-build-configuration.hpp"

#ifdef GX_USE_OPENGL_CLASS_3

#include "../../core/cr-static.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../gl/gl-types.hpp"
#include "../glc3.hpp"
#include <string>

#define GX_GLC3_UNIFORM_FAILED -1
#define GX_GLC3_TEXTURE_INDEX_FAILED -1

#ifdef GX_DEBUG_GL_CLASS_3
#define GX_DEBUG_GL_CLASS_3_GLSL
#endif

#define GX_GLC3_UNIFORM_TEXTURE_ARRAY(name, count)         \
    GX_GET_VAL_PRV(gl::sint, name, GX_GLC3_UNIFORM_FAILED) \
    GX_GET_ARR_PRV(gl::sint, name##_indices, count)

#define GX_GLC3_UNIFORM_TEXTURE(name)                      \
    GX_GET_VAL_PRV(gl::sint, name, GX_GLC3_UNIFORM_FAILED) \
    GX_GET_VAL_PRV(gl::sint, name##_index, GX_GLC3_TEXTURE_INDEX_FAILED)

#define GX_GLC3_UNIFORM(name, function)                      \
    GX_GET_VAL_PRV(gl::sint, name, GX_GLC3_UNIFORM_FAILED)   \
    void set_##name##_data(const float* data) const noexcept \
    {                                                        \
        gl::Loader::uniform##function;                       \
    }

#define GX_GLC3_UNIFORM_VECTOR(name, element_count, count) \
    GX_GLC3_UNIFORM(name, element_count##fv(name, count, data))

#define GX_GLC3_UNIFORM_FLOAT(name, count) \
    GX_GLC3_UNIFORM(name, 1fv(name, count, data))

#define GX_GLC3_UNIFORM_MATRIX(name, element_count, count) \
    GX_GLC3_UNIFORM(name, _matrix##element_count##fv(name, count, GL_FALSE, data))

#define GX_GLC3_GET_UNIFORM(shd, uniform)                    \
    uniform = shd->get_uniform_location(#uniform);           \
    if (GX_GLC3_UNIFORM_FAILED == uniform) {                 \
        GXLOGF("Failed to locate the uniform " << #uniform); \
    }

#define GX_GLC3_THIS_GET_UNIFORM(uniform) GX_GLC3_GET_UNIFORM(this, uniform)

#define GX_GLC3_SHADER_SET_TEXTURE_INDEX(x) \
    x##_index = texture_index;              \
    ++texture_index;

#define GX_GLC3_SHADER_SET_TEXTURE_INDEX_ARRAY(x) \
    for (auto& i : x##_indices) {                 \
        i = texture_index;                        \
        ++texture_index;                          \
    }

#define GX_GLC3_THIS_GET_UNIFORM_TEXTURE(uniform) \
    GX_GLC3_THIS_GET_UNIFORM(uniform)             \
    GX_GLC3_SHADER_SET_TEXTURE_INDEX(uniform)

#define GX_GLC3_THIS_GET_UNIFORM_TEXTURE_ARRAY(uniform) \
    GX_GLC3_GET_UNIFORM(this, uniform)                  \
    GX_GLC3_SHADER_SET_TEXTURE_INDEX_ARRAY(uniform)

#define GX_GLC3_SHADER_SET_TEXTURE_INDEX_STARTING gl::sint texture_index = 0;

#define GX_GLC3_SHADER_SET_TEXTURE_INDEX_UNIFORM(x) gl::Loader::uniform1i(x, x##_index);

#define GX_GLC3_SHADER_SET_TEXTURE_INDEX_ARRAY_UNIFORM(x) gl::Loader::uniform1iv(x, GX_COUNT_OF(x##_indices), x##_indices);

#define GX_GLC3_SHADER_SRC_DEFAULT_VERSION                                                                                                                               \
    "#version " << ((e->get_engine_type() == render::engine::Type::OPENGL_ES3) ? "300 es" : ((e->get_engine_type() == render::engine::Type::OPENGL_33) ? "330" : "430")) \
                << "\n"                                                                                                                                                  \
                << "#define GX_PI 3.141592653589793238\n"                                                                                                                \
                   "precision highp float;\n"                                                                                                                            \
                   "precision highp int;\n"                                                                                                                              \
                << ((e->get_engine_type() == render::engine::Type::OPENGL_ES3) ? "precision highp sampler2D;\nprecision highp samplerCube;\n" : "")

#define GX_GLC3_SHADER_SRC_DEFAULT_ATTRIBUTES  \
    "layout(location = 0) in vec3 position;\n" \
    "layout(location = 1) in vec3 normal;\n"   \
    "layout(location = 2) in vec4 tangent;\n"  \
    "layout(location = 3) in vec2 uv;\n"

#define GX_GLC3_SHADER_SRC_DEFAULT_VERTEX_STARTING \
    std::stringstream vertex_shader_code;          \
    vertex_shader_code << GX_GLC3_SHADER_SRC_DEFAULT_VERSION << GX_GLC3_SHADER_SRC_DEFAULT_ATTRIBUTES

#define GX_GLC3_SHADER_SRC_DEFAULT_FRAGMENT_STARTING \
    std::stringstream fragment_shader_code;          \
    fragment_shader_code << GX_GLC3_SHADER_SRC_DEFAULT_VERSION

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::shader {
class Shader {
protected:
    engine::Engine* const e;
    gl::uint shader_program = 0;
    gl::uint vertex_object = 0;
    gl::uint fragment_object = 0;

    void create_program() noexcept;
    void run() noexcept;
    void link() const noexcept;
    void validate() const noexcept;
    [[nodiscard]] gl::uint add_shader_to_program(const std::string& shd, gl::enumerated shader_type) const noexcept;
    void set_vertex_shader(const std::string& shd) noexcept;
    void set_fragment_shader(const std::string& shd) noexcept;
    static void end_program(gl::uint shader_program) noexcept;
    static void end_object(gl::uint shader_object) noexcept;

public:
    Shader(engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    virtual ~Shader() noexcept;
    /// On not found returns GX_SHADER_UNIFORM_FAILED
    [[nodiscard]] gl::sint get_uniform_location(const std::string& name) const noexcept;
    [[nodiscard]] gl::uint get_shader_program() const noexcept;
    virtual void bind() const noexcept;
};
}

#endif
#endif
