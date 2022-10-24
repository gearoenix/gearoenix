#ifndef GEAROENIX_GL_SHADER_HPP
#define GEAROENIX_GL_SHADER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../gx-gl-constants.hpp"
#include "../gx-gl-loader.hpp"
#include "../gx-gl-types.hpp"
#include <sstream>

#define GX_GL_UNIFORM_FAILED -1
#define GX_GL_TEXTURE_INDEX_FAILED -1

#define GX_GL_UNIFORM_TEXTURE_ARRAY(name, count)     \
    GX_GET_VAL_PRV(sint, name, GX_GL_UNIFORM_FAILED) \
    GX_GET_ARR_PRV(sint, name##_indices, count)

#define GX_GL_UNIFORM_TEXTURE(name)                   \
    GX_GET_VAL_PRV(sint, name, GX_GL_UNIFORM_FAILED); \
    GX_GET_VAL_PRV(sint, name##_index, GX_GL_TEXTURE_INDEX_FAILED)

#define GX_GL_UNIFORM(name, function)                              \
public:                                                            \
    void set_##name##_data(const float* const data) const noexcept \
    {                                                              \
        glUniform##function;                                       \
    }                                                              \
    GX_GET_VAL_PRV(sint, name, GX_GL_UNIFORM_FAILED)

#define GX_GL_UNIFORM_VECTOR(name, element_count, count) \
    GX_GL_UNIFORM(name, element_count##fv(name, count, data))

#define GX_GL_UNIFORM_FLOAT(name, count) \
    GX_GL_UNIFORM(name, 1fv(name, count, data))

#define GX_GL_UNIFORM_MATRIX(name, element_count, count) \
    GX_GL_UNIFORM(name, Matrix##element_count##fv(name, count, GL_FALSE, data))

#define GX_GL_GET_UNIFORM(shd, uniform)                        \
    uniform = shd->get_uniform_location(#uniform);             \
    if (GX_GL_UNIFORM_FAILED == uniform) {                     \
        GX_LOG_F("Failed to locate the uniform " << #uniform); \
    }

#define GX_GL_THIS_GET_UNIFORM(uniform) \
    GX_GL_GET_UNIFORM(this, uniform)    \
    static_assert(true, "")

#define GX_GL_SHADER_SET_TEXTURE_INDEX(x) \
    x##_index = texture_index;            \
    ++texture_index

#define GX_GL_SHADER_SET_TEXTURE_INDEX_ARRAY(x) \
    for (auto& i : x##_indices) {               \
        i = texture_index;                      \
        ++texture_index;                        \
    }

#define GX_GL_THIS_GET_UNIFORM_TEXTURE(uniform) \
    GX_GL_THIS_GET_UNIFORM(uniform);            \
    GX_GL_SHADER_SET_TEXTURE_INDEX(uniform)

#define GX_GL_THIS_GET_UNIFORM_TEXTURE_ARRAY(uniform) \
    GX_GL_THIS_GET_UNIFORM(uniform);                  \
    GX_GL_SHADER_SET_TEXTURE_INDEX_ARRAY(uniform)     \
    static_assert(true, "")

#define GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING sint texture_index = 0

#define GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(x) glUniform1i(x, x##_index)

#define GX_GL_SHADER_SET_TEXTURE_INDEX_ARRAY_UNIFORM(x) glUniform1iv(x, GX_COUNT_OF(x##_indices), x##_indices)

#define GX_GL_SHADER_SET_TEXTURE_INDEX_DYNAMIC_ARRAY_UNIFORM(x) \
    if (!x##_indices.empty())                                   \
    glUniform1iv(x, static_cast<sizei>(x##_indices.size()), x##_indices.data())

namespace gearoenix::gl {
struct Engine;
}

namespace gearoenix::gl::shader {
struct Shader {
    GX_GET_VAL_PRT(uint, shader_program, static_cast<uint>(-1));

protected:
    Engine& e;
    uint vertex_object = 0;
    uint fragment_object = 0;

    void run() noexcept;
    void link() const noexcept;
    void validate() const noexcept;
    [[nodiscard]] uint add_shader_to_program(const std::string& shd, enumerated shader_type) const noexcept;
    void set_vertex_shader(const std::string& shd) noexcept;
    void set_fragment_shader(const std::string& shd) noexcept;

public:
    explicit Shader(Engine& e) noexcept;
    Shader(const Shader&) = delete;
    Shader(Shader&&) noexcept;
    virtual ~Shader() noexcept;
    /// returns GX_SHADER_UNIFORM_FAILED when uniform not found.
    [[nodiscard]] sint get_uniform_location(const std::string& name) const noexcept;
    virtual void bind() const noexcept;
};
}

#endif
#endif
