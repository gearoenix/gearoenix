#include "gles2-shd-shader.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "../gles2.hpp"
#include <vector>

void gearoenix::gles2::shader::Shader::create_program()  noexcept
{
    shader_program = gl::Loader::create_program();
    if (shader_program == 0) {
        GXLOGF("Error creating shader program.")
    }
}

void gearoenix::gles2::shader::Shader::run() noexcept
{
    link();
    gl::Loader::use_program(shader_program);
}

void gearoenix::gles2::shader::Shader::link() noexcept
{
    gl::sint is_success = 0;
    gl::Loader::link_program(shader_program);
    gl::Loader::get_programiv(shader_program, GL_LINK_STATUS, &is_success);
    if (is_success == 0) {
        gl::sint max_length = 0;
        gl::Loader::get_programiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
        std::string info_log;
        info_log.resize(static_cast<std::size_t>(max_length));
        gl::Loader::get_program_info_log(shader_program, static_cast<gl::sizei>(max_length), nullptr, &(info_log[0]));
        GXLOGF("Error linking shader program: " << info_log)
    }
    gl::Loader::use_program(shader_program);
    position_attribute_location = gl::Loader::get_attrib_location(shader_program, "position");
    normal_attribute_location = gl::Loader::get_attrib_location(shader_program, "normal");
    tangent_attribute_location = gl::Loader::get_attrib_location(shader_program, "tangent");
    uv_attribute_location = gl::Loader::get_attrib_location(shader_program, "uv");
}

void gearoenix::gles2::shader::Shader::validate() noexcept
{
    gl::Loader::validate_program(shader_program);
    gl::sint is_success = 0;
    gl::Loader::get_programiv(shader_program, GL_VALIDATE_STATUS, &is_success);
    if (!is_success) {
        gl::sint max_length = 0;
        gl::Loader::get_programiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
        std::string info_log;
        info_log.resize(static_cast<std::size_t>(max_length));
        gl::Loader::get_program_info_log(shader_program, static_cast<gl::sizei>(max_length), nullptr, &(info_log[0]));
        GXLOGF("Invalid shader program: " << info_log)
    }
    gl::Loader::use_program(shader_program);
}

gearoenix::gl::uint gearoenix::gles2::shader::Shader::add_shader_to_program(const std::string& shd, const gl::enumerated shader_type) noexcept
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
    if (!success) {
        gl::sint sts_size;
        gl::Loader::get_shaderiv(shader_obj, GL_INFO_LOG_LENGTH, &sts_size);
        std::string info_log;
        info_log.resize(static_cast<std::size_t>(sts_size));
        gl::Loader::get_shader_info_log(shader_obj, static_cast<gl::sizei>(sts_size), nullptr, &(info_log[0]));
        GXLOGF("Error compiling shader type. Info: " << info_log)
    }
    gl::Loader::attach_shader(shader_program, shader_obj);
    return shader_obj;
}

gearoenix::gl::uint gearoenix::gles2::shader::Shader::set_vertex_shader(const std::string& shd) noexcept
{
    return add_shader_to_program(shd, GL_VERTEX_SHADER);
}

gearoenix::gl::uint gearoenix::gles2::shader::Shader::set_fragment_shader(const std::string& shd) noexcept
{
    return add_shader_to_program(shd, GL_FRAGMENT_SHADER);
}

void gearoenix::gles2::shader::Shader::end_program(const gl::uint shader_program) noexcept
{
    gl::Loader::delete_program(shader_program);
}

void gearoenix::gles2::shader::Shader::end_object(const gl::uint shader_object) noexcept
{
    gl::Loader::delete_shader(shader_object);
}

gearoenix::gles2::shader::Shader::Shader(engine::Engine*const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : e(e)
{
    e->get_function_loader()->load([c, this] {
        create_program();
    });
}

gearoenix::gles2::shader::Shader::~Shader() noexcept
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

gearoenix::gl::sint gearoenix::gles2::shader::Shader::get_uniform_location(const std::string& uname) const noexcept
{
    return gl::Loader::get_uniform_location(shader_program, &(uname[0]));
}

gearoenix::gl::uint gearoenix::gles2::shader::Shader::get_shader_program() const noexcept
{
    return shader_program;
}

void gearoenix::gles2::shader::Shader::bind() const noexcept
{

    gl::Loader::use_program(shader_program);
    if (position_attribute_location != -1)
        gl::Loader::enable_vertex_attrib_array(static_cast<gl::uint>(position_attribute_location));
    if (normal_attribute_location != -1)
        gl::Loader::enable_vertex_attrib_array(static_cast<gl::uint>(normal_attribute_location));
    if (tangent_attribute_location != -1)
        gl::Loader::enable_vertex_attrib_array(static_cast<gl::uint>(tangent_attribute_location));
    if (uv_attribute_location != -1)
        gl::Loader::enable_vertex_attrib_array(static_cast<gl::uint>(uv_attribute_location));

    if (position_attribute_location != -1)
        gl::Loader::vertex_attrib_pointer(static_cast<gl::uint>(position_attribute_location), 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), reinterpret_cast<void*>(0));
    if (normal_attribute_location != -1)
        gl::Loader::vertex_attrib_pointer(static_cast<gl::uint>(normal_attribute_location), 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    if (tangent_attribute_location != -1)
        gl::Loader::vertex_attrib_pointer(static_cast<gl::uint>(tangent_attribute_location), 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    if (uv_attribute_location != -1)
        gl::Loader::vertex_attrib_pointer(static_cast<gl::uint>(uv_attribute_location), 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), reinterpret_cast<void*>(10 * sizeof(float)));
}
#endif