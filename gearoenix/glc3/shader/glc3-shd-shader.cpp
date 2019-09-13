#include "glc3-shd-shader.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "../glc3.hpp"
#include <vector>

void gearoenix::glc3::shader::Shader::create_program() noexcept
{
    shader_program = gl::Loader::create_program();
    if (shader_program == 0) {
        GXLOGF("Error creating shader program.")
    }
}

void gearoenix::glc3::shader::Shader::run() noexcept
{
    link();
    gl::Loader::use_program(shader_program);
}

void gearoenix::glc3::shader::Shader::link() noexcept
{
    gl::sint is_success = 0;
    gl::Loader::link_program(shader_program);
    gl::Loader::get_programiv(shader_program, GL_LINK_STATUS, &is_success);
    gl::sint max_length = 0;
    gl::Loader::get_programiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
    std::string info_log;
    info_log.resize(static_cast<std::size_t>(max_length));
    gl::Loader::get_program_info_log(shader_program, static_cast<gl::sizei>(max_length), nullptr, &(info_log[0]));
    if (is_success == 0) {
        GXLOGF("Error linking shader program: " << info_log)
    } else {
        GXLOGD("Shader linking log is: " << info_log)
    }
    gl::Loader::use_program(shader_program);
}

void gearoenix::glc3::shader::Shader::validate() noexcept
{
    gl::Loader::validate_program(shader_program);
    gl::sint is_success = 0;
    gl::Loader::get_programiv(shader_program, GL_VALIDATE_STATUS, &is_success);
    gl::sint max_length = 0;
    gl::Loader::get_programiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
    std::string info_log;
    info_log.resize(static_cast<std::size_t>(max_length));
    gl::Loader::get_program_info_log(shader_program, static_cast<gl::sizei>(max_length), nullptr, &(info_log[0]));
    if (!is_success) {
        GXLOGF("Invalid shader program: " << info_log)
    } else {
        GXLOGD("Shader program log is: " << info_log)
    }
    gl::Loader::use_program(shader_program);
}

gearoenix::gl::uint gearoenix::glc3::shader::Shader::add_shader_to_program(const std::string& shd, const gl::enumerated shader_type) noexcept
{
    gl::uint shader_obj = gl::Loader::create_shader(shader_type);
    if (shader_obj == 0) {
        GXLOGF("Error creating shader type.")
    }
    const char* chtemp = shd.c_str();
    const auto uintemp = static_cast<gl::sint>(shd.length());
    gl::Loader::shader_source(shader_obj, 1, &(chtemp), &(uintemp));
    gl::Loader::compile_shader(shader_obj);
    gl::sint success;
    gl::Loader::get_shaderiv(shader_obj, GL_COMPILE_STATUS, &success);
    gl::sint sts_size;
    gl::Loader::get_shaderiv(shader_obj, GL_INFO_LOG_LENGTH, &sts_size);
    std::string info_log;
    info_log.resize(static_cast<std::size_t>(sts_size));
    gl::Loader::get_shader_info_log(shader_obj, static_cast<gl::sizei>(sts_size), nullptr, &(info_log[0]));
    if (!success) {
        GXLOGF("Error compiling shader type. Info: " << info_log << ", shader source: " << shd)
    } else {
        GXLOGD("Shader compiler log is: " << info_log)
    }
    gl::Loader::attach_shader(shader_program, shader_obj);
    return shader_obj;
}

gearoenix::gl::uint gearoenix::glc3::shader::Shader::set_vertex_shader(const std::string& shd) noexcept
{
    return add_shader_to_program(shd, GL_VERTEX_SHADER);
}

gearoenix::gl::uint gearoenix::glc3::shader::Shader::set_fragment_shader(const std::string& shd) noexcept
{
    return add_shader_to_program(shd, GL_FRAGMENT_SHADER);
}

void gearoenix::glc3::shader::Shader::end_program(const gl::uint shader_program) noexcept
{
    gl::Loader::delete_program(shader_program);
}

void gearoenix::glc3::shader::Shader::end_object(const gl::uint shader_object) noexcept
{
    gl::Loader::delete_shader(shader_object);
}

gearoenix::glc3::shader::Shader::Shader(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : e(e)
{
    e->get_function_loader()->load([c, this] {
        create_program();
    });
}

gearoenix::glc3::shader::Shader::~Shader() noexcept
{
    const gl::uint p = shader_program;
    const gl::uint v = vertex_object;
    const gl::uint f = fragment_object;
    e->get_function_loader()->load([p, v, f] {
        if (v != 0)
            end_object(v);
        if (f != 0)
            end_object(f);
        if (p != 0)
            end_program(p);
    });
    shader_program = 0;
    vertex_object = 0;
    fragment_object = 0;
}

gearoenix::gl::sint gearoenix::glc3::shader::Shader::get_uniform_location(const std::string& uname) const noexcept
{
    return gl::Loader::get_uniform_location(shader_program, &(uname[0]));
}

gearoenix::gl::uint gearoenix::glc3::shader::Shader::get_shader_program() const noexcept
{
    return shader_program;
}

void gearoenix::glc3::shader::Shader::bind() const noexcept
{
    gl::Loader::use_program(shader_program);
}
#endif