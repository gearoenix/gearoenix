#ifndef GEAROENIX_DIRECTX11_ENGINE_HPP
#define GEAROENIX_DIRECTX11_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../render/rnd-engine.hpp"
#include <d3d11.h>
#include <d3dcommon.h>
#include <dxgi.h>
#include <memory>
#ifdef PROFILING_MODE
#include <chrono>
#endif
namespace gearoenix {
namespace dx11 {
    namespace texture {
        class Texture2D;
        class Sampler;
    }
    class Engine : public render::Engine {
    private:
        unsigned int graphic_memory_size = 0;
        const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        texture::Sampler* sampler = nullptr;
        IDXGISwapChain* swapchain = nullptr;
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        ID3D11RenderTargetView* main_rtv = nullptr;
        ID3D11Texture2D* main_dsb = nullptr;
        ID3D11DepthStencilState* main_dss = nullptr;
        ID3D11DepthStencilView* main_dsv = nullptr;
        D3D11_VIEWPORT main_viewport;
        ID3D11RasterizerState* raster = nullptr;
        ID3D11RenderTargetView* shadow_rtv = nullptr;
        ID3D11Texture2D* shadow_dsb = nullptr;
        ID3D11DepthStencilView* shadow_dsv = nullptr;
        D3D11_VIEWPORT shadow_viewport;
        texture::Texture2D* shadow_txt = nullptr;
        void initial_shadow();
        void start_shadow_casting();
        void terminate_shadow();
#ifdef PROFILING_MODE
        unsigned int prof_frames_count = 0;
        std::chrono::high_resolution_clock::time_point prof_last_time_draw;
        double prof_frames_time = 0.0;
#endif
    public:
        Engine(system::Application* sys_app);
        ~Engine();
        static bool is_supported();
        void window_changed();
        void update();
        void terminate();
        render::texture::Texture2D* create_texture_2d(system::File* file, core::EndCaller<core::EndCallerIgnore> c);
        render::texture::Cube* create_texture_cube(system::File* file, core::EndCaller<core::EndCallerIgnore> c);
        render::buffer::Mesh* create_mesh(unsigned int vec, system::File* file, core::EndCaller<core::EndCallerIgnore> c);
        render::buffer::Uniform* create_uniform(unsigned int s, core::EndCaller<core::EndCallerIgnore> c);
        render::shader::Shader* create_shader(core::Id sid, system::File* file, core::EndCaller<core::EndCallerIgnore> c);
        render::shader::Resources* create_shader_resources(core::Id sid, render::pipeline::Pipeline* p, render::buffer::Uniform* ub, core::EndCaller<core::EndCallerIgnore> c);
        render::pipeline::Pipeline* create_pipeline(core::Id sid, core::EndCaller<core::EndCallerIgnore> c);
        ID3D11Device* get_device();
        const ID3D11Device* get_device() const;
        ID3D11DeviceContext* get_context();
        const ID3D11DeviceContext* get_context() const;
        texture::Sampler* get_sampler();
        const texture::Sampler* get_sampler() const;
    };
}
}
#endif
#endif
