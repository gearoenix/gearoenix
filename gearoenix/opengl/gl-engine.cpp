#include "gl-engine.hpp"
#ifdef USE_OPENGL
#include "../system/sys-log.hpp"
gearoenix::render::Engine::Engine(system::Application*)
{
    TODO;
}

gearoenix::render::Engine::~Engine()
{
    TODO;
}

void gearoenix::render::Engine::window_changed()
{
    TODO;
}

void gearoenix::render::Engine::update()
{
    TODO;
}

void gearoenix::render::Engine::terminate()
{
    TODO;
}

const gearoenix::render::Linker* gearoenix::render::Engine::get_linker() const
{
    TODO;
    return nullptr;
}

const gearoenix::render::device::Logical* gearoenix::render::Engine::get_logical_device() const
{
    TODO;
    return nullptr;
}

gearoenix::render::device::Logical* gearoenix::render::Engine::get_logical_device()
{
    TODO;
    return nullptr;
}

gearoenix::render::RenderPass* gearoenix::render::Engine::get_render_pass()
{
    TODO;
    return nullptr;
}

gearoenix::render::buffer::Manager* gearoenix::render::Engine::get_gpu_buffer_manager()
{
    TODO;
    return nullptr;
}

gearoenix::render::buffer::Manager* gearoenix::render::Engine::get_cpu_buffer_manager()
{
    TODO;
    return nullptr;
}

gearoenix::render::buffer::Manager* gearoenix::render::Engine::get_uniform_gpu_buffer_manager(unsigned int)
{
    TODO;
    return nullptr;
}

gearoenix::render::buffer::Manager* gearoenix::render::Engine::get_uniform_cpu_buffer_manager(unsigned int)
{
    TODO;
    return nullptr;
}

gearoenix::system::Application* gearoenix::render::Engine::get_system_application()
{
    TODO;
    return nullptr;
}

gearoenix::render::memory::Manager* gearoenix::render::Engine::get_v_memory_manager()
{
    TODO;
    return nullptr;
}

gearoenix::render::memory::Manager* gearoenix::render::Engine::get_cpu_memory_manager()
{
    TODO;
    return nullptr;
}

const gearoenix::render::pipeline::Manager* gearoenix::render::Engine::get_pipeline_manager() const
{
    TODO;
    return nullptr;
}

gearoenix::render::pipeline::Manager* gearoenix::render::Engine::get_pipeline_manager()
{
    TODO;
    return nullptr;
}

const gearoenix::render::texture::Sampler2D* gearoenix::render::Engine::get_sampler_2d() const
{
    TODO;
    return nullptr;
}

gearoenix::render::texture::Sampler2D* gearoenix::render::Engine::get_sampler_2d()
{
    TODO;
    return nullptr;
}

unsigned int gearoenix::render::Engine::get_frames_count() const
{
    TODO;
    return 0;
}

unsigned int gearoenix::render::Engine::get_current_frame_index() const
{
    TODO;
    return 0;
}

gearoenix::render::command::Buffer* gearoenix::render::Engine::get_current_command_buffer()
{
    TODO;
    return nullptr;
}

unsigned int gearoenix::render::Engine::load_scene(core::Id, std::function<void(unsigned int)>)
{
    TODO;
    return 0;
}

void gearoenix::render::Engine::push_todo(std::function<std::function<void()>(command::Buffer*)>)
{
    TODO;
}

#endif
