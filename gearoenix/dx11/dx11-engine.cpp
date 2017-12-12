#include "dx11-engine.hpp"
#ifdef USE_DIRECTX11
gearoenix::dx11::Engine::Engine(system::Application* sys_app): render::Engine(sys_app)
{}

gearoenix::dx11::Engine::~Engine()
{}

bool gearoenix::dx11::Engine::is_supported()
{
#ifdef IN_WINDOWS
	// TODO it must become better in future
	return true;
#else
	return false;
#endif
}

void gearoenix::dx11::Engine::window_changed()
{}

void gearoenix::dx11::Engine::update()
{}

void gearoenix::dx11::Engine::terminate()
{}

gearoenix::render::texture::Texture2D* gearoenix::dx11::Engine::create_texture_2d(system::File* file, std::shared_ptr<core::EndCaller> c)
{
	return nullptr;
}

gearoenix::render::texture::Cube* gearoenix::dx11::Engine::create_texture_cube(system::File* file, std::shared_ptr<core::EndCaller> c)
{
	return nullptr;
}

gearoenix::render::buffer::Mesh* gearoenix::dx11::Engine::create_mesh(unsigned int vec, system::File* file, std::shared_ptr<core::EndCaller> c)
{
	return nullptr;
}

gearoenix::render::buffer::Uniform* gearoenix::dx11::Engine::create_uniform(unsigned int s, std::shared_ptr<core::EndCaller> c)
{
	return nullptr;
}

gearoenix::render::shader::Shader* gearoenix::dx11::Engine::create_shader(core::Id sid, system::File* file, std::shared_ptr<core::EndCaller> c)
{
	return nullptr;
}

gearoenix::render::shader::Resources* gearoenix::dx11::Engine::create_shader_resources(core::Id sid, render::pipeline::Pipeline* p, render::buffer::Uniform* ub, std::shared_ptr<core::EndCaller> c)
{
	return nullptr;
}

gearoenix::render::pipeline::Pipeline* gearoenix::dx11::Engine::create_pipeline(core::Id sid, std::shared_ptr<core::EndCaller> c)
{
	return nullptr;
}

#endif