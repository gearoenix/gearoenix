#include "dx11-engine.hpp"
#ifdef USE_DIRECTX11
#include "../system/sys-log.hpp"
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <vector>
#include "../system/sys-app.hpp"

gearoenix::dx11::Engine::Engine(system::Application* sys_app): render::Engine(sys_app)
{
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapter_output;
	unsigned int num_modes, i, numerator, denominator, string_length;
	DXGI_ADAPTER_DESC adapter_desc;
	int error;
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	D3D_FEATURE_LEVEL feature_level;
	ID3D11Texture2D* back_buffer_ptr;
	D3D11_TEXTURE2D_DESC depth_buffer_desc;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	D3D11_RASTERIZER_DESC raster_desc;
	D3D11_VIEWPORT viewport;
	float field_of_view, screen_aspect;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory))) {
		GXLOGF("CreateDXGIFactory failed.");
	}
	if (FAILED(factory->EnumAdapters(0, &adapter))) {
		GXLOGF("EnumAdapters failed.");
	}
	if (FAILED(adapter->EnumOutputs(0, &adapter_output))) {
		GXLOGF("EnumOutputs failed.");
	}
	if (FAILED(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, NULL))) {
		GXLOGF("GetDisplayModeList failed.");
	}
	std::vector<DXGI_MODE_DESC> display_mode_list(num_modes);
	if (FAILED(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, &(display_mode_list[0])))) {
		GXLOGF("GetDisplayModeList failed.");
	}
	for (i = 0; i<num_modes; i++) {
		if (display_mode_list[i].Width == sysapp->get_width()) {
			if (display_mode_list[i].Height == sysapp->get_height()) {
				numerator = display_mode_list[i].RefreshRate.Numerator;
				denominator = display_mode_list[i].RefreshRate.Denominator;
			}
		}
	}
}

gearoenix::dx11::Engine::~Engine()
{}

bool gearoenix::dx11::Engine::is_supported()
{
	// TODO it must become better in future
	return true;
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