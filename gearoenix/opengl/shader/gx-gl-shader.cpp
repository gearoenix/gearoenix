#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../gx-gl-check.hpp"
#include "../gx-gl-engine.hpp"

namespace {
std::string shader_version;
std::string shader_common_starter;

constexpr char shader_common_constants[] = R"SHADER(
#define gx_pi 3.141592653589793238
)SHADER";

constexpr auto optional_shader_extensions = std::array {
    "GL_OES_texture_float",
    "GL_OES_texture_float_linear",
    "OES_texture_float",
    "OES_texture_float_linear",
};

constexpr char shader_common_precisions[] = R"SHADER(
precision highp float;
precision highp int;
precision highp sampler2D;
precision highp samplerCube;
precision highp sampler2DShadow;
)SHADER";
}

void gearoenix::gl::shader::Shader::run()
{
    link();
    glUseProgram(shader_program);
}

void gearoenix::gl::shader::Shader::link() const
{
    sint is_success = 0;
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &is_success);
    sint max_length = 0;
    glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
    std::string info_log;
    info_log.resize(static_cast<std::uint32_t>(max_length));
    glGetProgramInfoLog(shader_program, static_cast<sizei>(max_length), nullptr, &(info_log[0]));
    if (is_success == 0) {
        info_log[max_length - 1] = '\n';
        GX_LOG_F("Error linking shader program: " << info_log);
    }
    if (info_log.size() > 5) {
        info_log[max_length - 1] = '\n';
        GX_LOG_D("Shader linking log is: " << info_log);
    }
    glUseProgram(shader_program);
}

void gearoenix::gl::shader::Shader::validate() const
{
    glValidateProgram(shader_program);
    sint is_success = 0;
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &is_success);
    sint max_length = 0;
    glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
    std::string info_log;
    info_log.resize(static_cast<std::uint32_t>(max_length));
    glGetProgramInfoLog(shader_program, static_cast<sizei>(max_length), nullptr, &(info_log[0]));
    if (!is_success) {
        info_log[max_length - 1] = '\n';
        GX_LOG_F("Invalid shader program: " << info_log);
    } else if (!info_log.empty()) {
        info_log[max_length - 1] = '\n';
        GX_LOG_D("Shader program log is: " << info_log);
    }
    glUseProgram(shader_program);
}

gearoenix::gl::uint gearoenix::gl::shader::Shader::add_shader_to_program(const std::string& shd, const enumerated shader_type) const
{
    uint shader_obj = glCreateShader(shader_type);
    if (shader_obj == 0) {
        GX_LOG_F("Error creating shader type.");
    }
    const char* const shader_src_ptr = shd.c_str();
    const auto shader_src_len = static_cast<sint>(shd.length());
    glShaderSource(shader_obj, 1, &(shader_src_ptr), &(shader_src_len));
    glCompileShader(shader_obj);
    sint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);
    sint sts_size;
    glGetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &sts_size);
    std::string info_log;
    info_log.resize(static_cast<std::uint32_t>(sts_size));
    glGetShaderInfoLog(shader_obj, static_cast<sizei>(sts_size), nullptr, &(info_log[0]));
    if (!success) {
        info_log[sts_size - 1] = '\n';
        GX_LOG_F("Error compiling shader. Info: " << info_log << ", shader source: \n"
                                                  << shd);
    } else if (!info_log.empty()) {
        info_log[sts_size - 1] = '\n';
        GX_LOG_D("Shader compiler log is: " << info_log);
    }
    glAttachShader(shader_program, shader_obj);
    return shader_obj;
}

void gearoenix::gl::shader::Shader::set_vertex_shader(const std::string& shd)
{
    vertex_object = add_shader_to_program(shd, GL_VERTEX_SHADER);
}

void gearoenix::gl::shader::Shader::set_fragment_shader(const std::string& shd)
{
    fragment_object = add_shader_to_program(shd, GL_FRAGMENT_SHADER);
}

gearoenix::gl::shader::Shader::Shader()
    : shader_program(glCreateProgram())
{
    GX_ASSERT_D(core::Singleton<Engine>::get().get_jobs_thread_id() == std::this_thread::get_id());
    if (shader_program == 0) {
        GX_LOG_F("Error creating shader program.");
    }
}

gearoenix::gl::shader::Shader::Shader(Shader&& o) noexcept
    : shader_program(o.shader_program)
    , vertex_object(o.vertex_object)
    , fragment_object(o.fragment_object)
{
    o.shader_program = static_cast<uint>(-1);
    o.vertex_object = static_cast<uint>(-1);
    o.fragment_object = static_cast<uint>(-1);
}

gearoenix::gl::shader::Shader& gearoenix::gl::shader::Shader::operator=(Shader&& o) noexcept
{
    shader_program = o.shader_program;
    vertex_object = o.vertex_object;
    fragment_object = o.fragment_object;
    o.shader_program = static_cast<uint>(-1);
    o.vertex_object = static_cast<uint>(-1);
    o.fragment_object = static_cast<uint>(-1);
    return *this;
}

gearoenix::gl::shader::Shader::~Shader()
{
    core::job::send_job(core::Singleton<Engine>::get().get_jobs_thread_id(), [vo = vertex_object, fo = fragment_object, sp = shader_program] {
        GX_GL_CHECK_D;
        if (static_cast<uint>(-1) != vo) {
            glDeleteShader(vo);
        }
        if (static_cast<uint>(-1) != fo) {
            glDeleteShader(fo);
        }
        if (static_cast<uint>(-1) != sp) {
            glDeleteProgram(sp);
        }
        GX_GL_CHECK_D;
    });
    vertex_object = fragment_object = shader_program = static_cast<uint>(-1);
}

gearoenix::gl::sint gearoenix::gl::shader::Shader::get_uniform_location(const std::string& uname) const
{
    return glGetUniformLocation(shader_program, &(uname[0]));
}

void gearoenix::gl::shader::Shader::bind(uint& current_shader) const
{
    glUseProgram(shader_program);
    current_shader = shader_program;
}

void gearoenix::gl::shader::Shader::set_profile(const bool is_es, const int major, const int minor)
{
    shader_version.clear();
    shader_version += "#version ";
    shader_version += std::to_string(major);
#if GX_PLATFORM_WEBASSEMBLY
    shader_version += '0';
#else
    shader_version += is_es ? "0" : std::to_string(minor);
#endif
    shader_version += "0 ";
    shader_version += is_es ? "es" : "core";

    shader_common_starter.clear();
    shader_common_starter += shader_version;
    shader_common_starter += "\n";

    for (const auto& ext : optional_shader_extensions) {
        if (extension_exists(ext)) {
            shader_common_starter += "#extension ";
            shader_common_starter += ext;
            shader_common_starter += " : enable\n";
        } else {
            GX_LOG_D("Shader extension " << ext << " is not supported.");
        }
    }

    shader_common_starter += is_es ? shader_common_precisions : "";
    shader_common_starter += shader_common_constants;
}

const std::string& gearoenix::gl::shader::Shader::get_shader_version()
{
    return shader_version;
}

const std::string& gearoenix::gl::shader::Shader::get_common_shader_starter()
{
    return shader_common_starter;
}

#endif