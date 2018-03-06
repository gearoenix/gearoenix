#include "dx11-engine.hpp"
#ifdef USE_DIRECTX11
#include "../core/cr-static.hpp"
#include "../core/event/cr-ev-window-resize.hpp"
#include "../math/math-matrix.hpp"
#include "../math/math-vector.hpp"
#include "../physics/phs-engine.hpp"
#include "../render/pipeline/rnd-pip-manager.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"
#include "buffer/dx11-buf-mesh.hpp"
#include "buffer/dx11-buf-uniform.hpp"
#include "dx11-check.hpp"
#include "pipeline/dx11-pip-pipeline.hpp"
#include "shader/dx11-shd-depth.hpp"
#include "shader/dx11-shd-directional-colored-speculated-baked-full-opaque.hpp"
#include "shader/dx11-shd-directional-d2-speculated-baked-full-opaque.hpp"
#include "shader/dx11-shd-directional-d2-speculated-nonreflective-full-opaque.hpp"
#include "shader/dx11-shd-directional-d2-speculated-nonreflective-shadowless-opaque.hpp"
#include "shader/dx11-shd-font-colored.hpp"
#include "shader/dx11-shd-shadeless-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "shader/dx11-shd-shadeless-cube-matte-nonreflective-shadowless-opaque.hpp"
#include "shader/dx11-shd-shadeless-d2-matte-nonreflective-shadowless-opaque.hpp"
#include "shader/dx11-shd-skybox-basic.hpp"
#include "texture/dx11-txt-2d.hpp"
#include "texture/dx11-txt-cube.hpp"
#include "texture/dx11-txt-sampler.hpp"
#include <cstdlib>
#include <d3dcompiler.h>
#include <vector>

#define SHADOW_WIDTH 1024

void gearoenix::dx11::Engine::initial_screen()
{
    ID3D11Texture2D* back_buffer_ptr;
    GXHRCHK(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr));
    context->OMSetRenderTargets(0, nullptr, nullptr);
    GXHRCHK(device->CreateRenderTargetView(back_buffer_ptr, NULL, &main_rtv));
    back_buffer_ptr->Release();
    back_buffer_ptr = nullptr;
    D3D11_TEXTURE2D_DESC depth_buffer_desc;
    GXSETZ(depth_buffer_desc);
    depth_buffer_desc.Width = sysapp->get_width();
    depth_buffer_desc.Height = sysapp->get_height();
    depth_buffer_desc.MipLevels = 1;
    depth_buffer_desc.ArraySize = 1;
    depth_buffer_desc.Format = DXGI_FORMAT_D32_FLOAT;
    depth_buffer_desc.SampleDesc = sample;
    depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_desc.CPUAccessFlags = 0;
    depth_buffer_desc.MiscFlags = 0;
    GXHRCHK(device->CreateTexture2D(&depth_buffer_desc, NULL, &main_dsb));
    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
    GXSETZ(depth_stencil_view_desc);
    depth_stencil_view_desc.Format = depth_buffer_desc.Format;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;
    GXHRCHK(device->CreateDepthStencilView(main_dsb, &depth_stencil_view_desc, &main_dsv));
    context->OMSetRenderTargets(1, &main_rtv, main_dsv);
    main_viewport.Width = (float)sysapp->get_width();
    main_viewport.Height = (float)sysapp->get_height();
}

void gearoenix::dx11::Engine::terminate_screen()
{
    if (nullptr != main_rtv)
        main_rtv->Release();
    main_rtv = nullptr;
    if (nullptr != main_dsb)
        main_dsb->Release();
    main_dsb = nullptr;
    if (nullptr != main_dsv)
        main_dsv->Release();
    main_dsv = nullptr;
}

void gearoenix::dx11::Engine::initial_shadow()
{
    D3D11_TEXTURE2D_DESC tdesc;
    GXSETZ(tdesc);
    tdesc.Width = SHADOW_WIDTH;
    tdesc.Height = SHADOW_WIDTH;
    tdesc.MipLevels = 1;
    tdesc.ArraySize = 1;
    tdesc.Format = DXGI_FORMAT_R32_FLOAT;
    tdesc.SampleDesc.Count = 1;
    tdesc.Usage = D3D11_USAGE_DEFAULT;
    tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    ID3D11Texture2D* txt = nullptr;
    GXHRCHK(device->CreateTexture2D(&tdesc, nullptr, &txt));
    D3D11_RENDER_TARGET_VIEW_DESC rdesc;
    GXSETZ(rdesc);
    rdesc.Format = tdesc.Format;
    rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rdesc.Texture2D.MipSlice = 0;
    GXHRCHK(device->CreateRenderTargetView(txt, &rdesc, &shadow_rtv));
    D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
    GXSETZ(sdesc);
    sdesc.Format = tdesc.Format;
    sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    sdesc.Texture2D.MipLevels = 1;
    ID3D11ShaderResourceView* shadow_srv;
    GXHRCHK(device->CreateShaderResourceView(txt, &sdesc, &shadow_srv));
    shadow_txt = new texture::Texture2D(this, shadow_srv);
    GXSETZ(tdesc);
    tdesc.Width = SHADOW_WIDTH;
    tdesc.Height = SHADOW_WIDTH;
    tdesc.MipLevels = 1;
    tdesc.ArraySize = 1;
    tdesc.Format = DXGI_FORMAT_D32_FLOAT;
    tdesc.SampleDesc.Count = 1;
    tdesc.Usage = D3D11_USAGE_DEFAULT;
    tdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    GXHRCHK(device->CreateTexture2D(&tdesc, nullptr, &shadow_dsb));
    D3D11_DEPTH_STENCIL_VIEW_DESC ddesc;
    GXSETZ(ddesc);
    ddesc.Format = tdesc.Format;
    ddesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    ddesc.Texture2D.MipSlice = 0;
    GXHRCHK(device->CreateDepthStencilView(shadow_dsb, &ddesc, &shadow_dsv));
    shadow_viewport.Width = (float)SHADOW_WIDTH;
    shadow_viewport.Height = (float)SHADOW_WIDTH;
    shadow_viewport.MinDepth = 0.0f;
    shadow_viewport.MaxDepth = 1.0f;
    shadow_viewport.TopLeftX = 0.0f;
    shadow_viewport.TopLeftY = 0.0f;
    txt->Release();
    D3D11_BLEND_DESC blend_desc;
    GXSETZ(blend_desc);
    blend_desc.RenderTarget[0].BlendEnable = false;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    GXHRCHK(device->CreateBlendState(&blend_desc, &shadow_bs));
}

void gearoenix::dx11::Engine::start_shadow_casting()
{
    ID3D11ShaderResourceView* const null[2] = { nullptr, nullptr };
    for (UINT i = 0; i < 2; ++i)
        context->PSSetShaderResources(i, 2, null);
    context->OMSetRenderTargets(1, &shadow_rtv, shadow_dsv);
    context->RSSetViewports(1, &shadow_viewport);
    context->ClearRenderTargetView(shadow_rtv, clear_color);
    context->ClearDepthStencilView(shadow_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
    context->OMSetBlendState(shadow_bs, clear_color, 0XFFFFFFFF);
}

void gearoenix::dx11::Engine::terminate_shadow()
{
    shadow_dsv->Release();
    shadow_dsv = nullptr;
    shadow_dsb->Release();
    shadow_dsb = nullptr;
    shadow_rtv->Release();
    shadow_rtv = nullptr;
    delete shadow_txt;
    shadow_txt = nullptr;
    shadow_bs->Release();
    shadow_bs = nullptr;
}

gearoenix::dx11::Engine::Engine(system::Application* sys_app)
    : render::Engine(sys_app)
{
    GXSETZ(sample);
    sample.Count = 1;
    IDXGIFactory* factory = nullptr;
    IDXGIAdapter* adapter = nullptr;
    IDXGIOutput* adapter_output = nullptr;
    UINT numerator = 0U, denominator = 0U;
    GXHRCHK(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));
    for (UINT adapter_index = 0;
         factory->EnumAdapters(adapter_index, &adapter) != DXGI_ERROR_NOT_FOUND;
         ++adapter_index, adapter->Release(), adapter = nullptr) {
        DXGI_ADAPTER_DESC adapter_desc;
        GXHRCHK(adapter->GetDesc(&adapter_desc));
        graphic_memory_size = (unsigned int)adapter_desc.DedicatedVideoMemory;
        char video_card_description[128];
        size_t strlen;
        if (wcstombs_s(&strlen, video_card_description, 128, adapter_desc.Description, 128) != 0) {
            UNEXPECTED;
        }
        for (size_t i = strlen; i < 128; ++i)
            video_card_description[i] = 0;
        GXLOGD("Video Card Description: " << video_card_description);
        for (UINT adpout_index = 0;
             adapter->EnumOutputs(adpout_index, &adapter_output) != DXGI_ERROR_NOT_FOUND;
             ++adpout_index, adapter_output->Release(), adapter_output = nullptr) {
            UINT num_modes = 0;
            if (FAILED(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, NULL))) {
                continue;
            }
            if (num_modes == 0) {
                continue;
            }
            std::vector<DXGI_MODE_DESC> display_mode_list(num_modes);
            if (FAILED(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, &(display_mode_list[0])))) {
                continue;
            }
            for (UINT i = 0; i < num_modes; i++) {
                if (display_mode_list[i].Width == sysapp->get_width()) {
                    if (display_mode_list[i].Height == sysapp->get_height()) {
                        numerator = display_mode_list[i].RefreshRate.Numerator;
                        denominator = display_mode_list[i].RefreshRate.Denominator;
                    }
                }
            }
            goto adapter_found_label;
        }
    }
adapter_found_label:
    if (adapter_output != nullptr)
        adapter_output->Release();
    if (adapter != nullptr)
        adapter->Release();
    if (factory != nullptr)
        factory->Release();
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    GXSETZ(swap_chain_desc);
    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.BufferDesc.Width = sysapp->get_width();
    swap_chain_desc.BufferDesc.Height = sysapp->get_height();
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = numerator;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = denominator;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = sysapp->get_window();
    swap_chain_desc.SampleDesc = sample;
#ifdef GEAROENIX_FULLSCREEN
    swap_chain_desc.Windowed = false;
#else
    swap_chain_desc.Windowed = true;
#endif
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.Flags = 0;

    UINT device_flag =
#ifdef DEBUG_MODE
        D3D11_CREATE_DEVICE_DEBUG;
#else
        D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT;
#endif
    const D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
    D3D_DRIVER_TYPE driver_type = D3D_DRIVER_TYPE_HARDWARE;
    if (FAILED(D3D11CreateDevice(
            nullptr, driver_type, nullptr, device_flag,
            &feature_level, 1, D3D11_SDK_VERSION,
            &device, nullptr, nullptr))) {
#ifdef DEBUG_MODE
        driver_type = D3D_DRIVER_TYPE_REFERENCE;
        GXHRCHK(D3D11CreateDevice(
            nullptr, driver_type, nullptr, device_flag,
            &feature_level, 1, D3D11_SDK_VERSION,
            &device, nullptr, nullptr));
#endif
    }
    for (unsigned int i = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i > 0; --i) {
        swap_chain_desc.SampleDesc.Count = i;
        UINT sample_quality;
        device->CheckMultisampleQualityLevels(
            swap_chain_desc.BufferDesc.Format,
            i, &sample_quality);
        if (sample_quality > 0) {
            --sample_quality;
            swap_chain_desc.SampleDesc.Quality = sample_quality;
            swap_chain_desc.SampleDesc.Count = i;
            GXLOGD("count " << i << ",  quality: " << sample_quality);
            break;
        }
    }
    sample = swap_chain_desc.SampleDesc;
    device->Release();
    device = nullptr;
    GXHRCHK(D3D11CreateDeviceAndSwapChain(
        nullptr, driver_type, nullptr, device_flag,
        &feature_level, 1, D3D11_SDK_VERSION, &swap_chain_desc, &swapchain,
        &device, nullptr, &context));
    initial_screen();
    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
    GXSETZ(depth_stencil_desc);
    depth_stencil_desc.DepthEnable = true;
    depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
    depth_stencil_desc.StencilEnable = true;
    depth_stencil_desc.StencilReadMask = 0xFF;
    depth_stencil_desc.StencilWriteMask = 0xFF;
    depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    GXHRCHK(device->CreateDepthStencilState(&depth_stencil_desc, &main_dss));
    context->OMSetDepthStencilState(main_dss, 1);
    D3D11_BLEND_DESC blend_desc;
    GXSETZ(blend_desc);
    blend_desc.RenderTarget[0].BlendEnable = true;
    blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = 0X0F;
    GXHRCHK(device->CreateBlendState(&blend_desc, &main_bs));
    D3D11_RASTERIZER_DESC raster_desc;
    GXSETZ(raster_desc);
    raster_desc.AntialiasedLineEnable = true;
    raster_desc.CullMode = D3D11_CULL_BACK;
    raster_desc.DepthBiasClamp = 0.0f;
    raster_desc.DepthClipEnable = true;
    raster_desc.FillMode = D3D11_FILL_SOLID;
    raster_desc.FrontCounterClockwise = true;
    raster_desc.MultisampleEnable = true;
    raster_desc.ScissorEnable = false;
    raster_desc.SlopeScaledDepthBias = 0.0f;
    GXHRCHK(device->CreateRasterizerState(&raster_desc, &raster));
    context->RSSetState(raster);
    main_viewport.MinDepth = 0.0f;
    main_viewport.MaxDepth = 1.0f;
    main_viewport.TopLeftX = 0.0f;
    main_viewport.TopLeftY = 0.0f;
    context->RSSetViewports(1, &main_viewport);
    sampler = new texture::Sampler(this);
    pipmgr = new render::pipeline::Manager(this);
    initial_shadow();
}

gearoenix::dx11::Engine::~Engine()
{
    terminate();
}

bool gearoenix::dx11::Engine::is_supported()
{
    GXLOGE("TODO it must become better in future.");
    return true;
}

void gearoenix::dx11::Engine::window_changed()
{
    TODO;
}

void gearoenix::dx11::Engine::update()
{
    context->ClearRenderTargetView(main_rtv, clear_color);
    do_load_functions();
    physics_engine->wait();
    for (const std::pair<core::Id, std::shared_ptr<render::scene::Scene>>& id_scene : loaded_scenes) {
        const std::shared_ptr<render::scene::Scene>& scene = id_scene.second;
        start_shadow_casting();
        scene->cast_shadow();
        context->OMSetRenderTargets(1, &main_rtv, main_dsv);
        context->RSSetViewports(1, &main_viewport);
        context->ClearDepthStencilView(main_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
        context->OMSetBlendState(main_bs, clear_color, 0XFFFFFFFF);
		scene->draw_sky();
		context->ClearDepthStencilView(main_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
        scene->draw(shadow_txt);
    }
    physics_engine->update();
    swapchain->Present(1, 0);
}

void gearoenix::dx11::Engine::terminate()
{
    if (swapchain == nullptr)
        return;
    terminate_shadow();
    terminate_screen();
    main_bs->Release();
    main_bs = nullptr;
    swapchain->SetFullscreenState(false, NULL);
    delete sampler;
    sampler = nullptr;
    raster->Release();
    raster = nullptr;
    main_dss->Release();
    main_dss = nullptr;
    context->Release();
    context = nullptr;
    device->Release();
    device = nullptr;
    swapchain->Release();
    swapchain = nullptr;
}

gearoenix::render::texture::Texture2D* gearoenix::dx11::Engine::create_texture_2d(system::stream::Stream* file, core::EndCaller<core::EndCallerIgnore> c)
{
    return new texture::Texture2D(file, this, c);
}

gearoenix::render::texture::Cube* gearoenix::dx11::Engine::create_texture_cube(system::stream::Stream* file, core::EndCaller<core::EndCallerIgnore> c)
{
    return new texture::Cube(file, this, c);
}

gearoenix::render::buffer::Mesh* gearoenix::dx11::Engine::create_mesh(unsigned int vec, system::stream::Stream* file, core::EndCaller<core::EndCallerIgnore> c)
{
    return new buffer::Mesh(vec, file, this, c);
}

gearoenix::render::buffer::Uniform* gearoenix::dx11::Engine::create_uniform(unsigned int s, core::EndCaller<core::EndCallerIgnore> c)
{
    return new buffer::Uniform(s, this, c);
}

gearoenix::render::shader::Shader* gearoenix::dx11::Engine::create_shader(core::Id sid, system::stream::Stream* file, core::EndCaller<core::EndCallerIgnore> c)
{
    render::shader::Id shader_id = (render::shader::Id)sid;
    switch (shader_id) {
    case render::shader::DEPTH_POS:
    case render::shader::DEPTH_POS_NRM:
    case render::shader::DEPTH_POS_NRM_UV:
    case render::shader::DEPTH_POS_UV:
        return new shader::Depth(this, c);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE:
        return new shader::DirectionalColoredSpeculatedBakedFullOpaque(this, c);
    case render::shader::DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE:
        return new shader::DirectionalD2SpeculatedBakedFullOpaque(this, c);
    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE:
        return new shader::DirectionalD2SpeculatedNonreflectiveFullOpaque(this, c);
    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque(this, c);
    case render::shader::FONT_COLORED:
        return new shader::FontColored(this, c);
    case render::shader::Id::SHADELESS_COLORED_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case render::shader::Id::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
    case render::shader::Id::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
        return new shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque(this, c);
    case render::shader::Id::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque(this, c);
    case render::shader::Id::SHADELESS_D2_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case render::shader::Id::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
	case render::shader::Id::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
		return new shader::ShadelessD2MatteNonreflectiveShadowlessOpaque(this, c);
	case render::shader::Id::SKYBOX_BASIC:
		return new shader::SkyboxBasic(this, c);
    default:
        UNIMPLEMENTED;
        break;
    }
    return nullptr;
}

gearoenix::render::shader::Resources* gearoenix::dx11::Engine::create_shader_resources(core::Id sid, render::pipeline::Pipeline* p, render::buffer::Uniform* ub, core::EndCaller<core::EndCallerIgnore> c)
{
    pipeline::Pipeline* pip = reinterpret_cast<pipeline::Pipeline*>(p);
    buffer::Uniform* u = reinterpret_cast<buffer::Uniform*>(ub);
    render::shader::Id shader_id = (render::shader::Id)sid;
    switch (shader_id) {
    case render::shader::DEPTH_POS:
    case render::shader::DEPTH_POS_NRM:
    case render::shader::DEPTH_POS_NRM_UV:
    case render::shader::DEPTH_POS_UV:
        return new shader::Depth::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE:
        return new shader::DirectionalColoredSpeculatedBakedFullOpaque::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE:
        return new shader::DirectionalD2SpeculatedBakedFullOpaque::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE:
        return new shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::FONT_COLORED:
        return new shader::FontColored::Resources(this, pip, u);
    case render::shader::Id::SHADELESS_COLORED_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case render::shader::Id::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
    case render::shader::Id::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
        return new shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::Id::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::Id::SHADELESS_D2_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case render::shader::Id::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
    case render::shader::Id::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
        return new shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
	case render::shader::Id::SKYBOX_BASIC:
		return new shader::SkyboxBasic::Resources(this, pip, u);
    default:
        UNIMPLEMENTED;
        break;
    }
    return nullptr;
}

gearoenix::render::pipeline::Pipeline* gearoenix::dx11::Engine::create_pipeline(core::Id sid, core::EndCaller<core::EndCallerIgnore> c)
{
    return new pipeline::Pipeline(sid, this, c);
}

ID3D11Device* gearoenix::dx11::Engine::get_device()
{
    return device;
}

const ID3D11Device* gearoenix::dx11::Engine::get_device() const
{
    return device;
}

ID3D11DeviceContext* gearoenix::dx11::Engine::get_context()
{
    return context;
}

const ID3D11DeviceContext* gearoenix::dx11::Engine::get_context() const
{
    return context;
}

gearoenix::dx11::texture::Sampler* gearoenix::dx11::Engine::get_sampler()
{
    return sampler;
}

const gearoenix::dx11::texture::Sampler* gearoenix::dx11::Engine::get_sampler() const
{
    return sampler;
}

void gearoenix::dx11::Engine::on_event(core::event::Event& e)
{
    render::Engine::on_event(e);
    if (e.get_type() == core::event::Event::From::WINDOW_RESIZE) {
        terminate_screen();
        GXHRCHK(swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
        initial_screen();
    }
}

#endif
