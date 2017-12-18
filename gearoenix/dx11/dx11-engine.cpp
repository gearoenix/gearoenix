#include "dx11-engine.hpp"
#ifdef USE_DIRECTX11
#include "../core/cr-static.hpp"
#include "../math/math-matrix.hpp"
#include "../math/math-vector.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"
#include <cstdlib>
#include <d3dcompiler.h>
#include <vector>
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace gearoenix {
struct VertexType {
    math::Vec3 position;
    math::Vec3 color;
};
static ID3D11Buffer* p_vertex_buffer = nullptr;
static ID3D11Buffer* p_index_buffer = nullptr;
static ID3D11Buffer* p_uniform_buffer = nullptr;
static ID3D11VertexShader* p_vertex_shader = nullptr;
static ID3D11PixelShader* p_fragment_shader = nullptr;
static ID3D11InputLayout* p_input_layout = nullptr;
struct UniformBufferType {
    math::Mat4x4 mvp;
};
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
gearoenix::dx11::Engine::Engine(system::Application* sys_app)
    : render::Engine(sys_app)
{
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapter_output;
    unsigned int num_modes, i, numerator, denominator;
    DXGI_ADAPTER_DESC adapter_desc;
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    const D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
    ID3D11Texture2D* back_buffer_ptr;
    D3D11_TEXTURE2D_DESC depth_buffer_desc;
    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
    D3D11_RASTERIZER_DESC raster_desc;
    D3D11_VIEWPORT viewport;
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
    for (i = 0; i < num_modes; i++) {
        if (display_mode_list[i].Width == sysapp->get_width()) {
            if (display_mode_list[i].Height == sysapp->get_height()) {
                numerator = display_mode_list[i].RefreshRate.Numerator;
                denominator = display_mode_list[i].RefreshRate.Denominator;
            }
        }
    }

    if (FAILED(adapter->GetDesc(&adapter_desc))) {
        GXLOGF("getting adapter description failed.");
    }
    graphic_memory_size = (unsigned int)adapter_desc.DedicatedVideoMemory;
    char video_card_description[128];
    size_t strlen;
    if (wcstombs_s(&strlen, video_card_description, 128, adapter_desc.Description, 128) != 0) {
        UNEXPECTED;
    }
    for (size_t i = strlen; i < 128; ++i)
        video_card_description[i] = 0;
    GXLOGD("Video Card Description: " << video_card_description);

    adapter_output->Release();
    adapter->Release();
    factory->Release();

    setz(swap_chain_desc);
    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.BufferDesc.Width = sysapp->get_width();
    swap_chain_desc.BufferDesc.Height = sysapp->get_height();
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = numerator;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = denominator;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = sysapp->get_window();
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
#ifdef GEAROENIX_FULLSCREEN
    swap_chain_desc.Windowed = false;
#else
    swap_chain_desc.Windowed = true;
#endif
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.Flags = 0;

    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &feature_level, 1,
            D3D11_SDK_VERSION, &swap_chain_desc, &p_swapchain, &p_device, NULL, &p_immediate_context))) {
        GXLOGF("Can not create requested contex.");
    }
    if (FAILED(p_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr))) {
        UNEXPECTED;
    }
    if (FAILED(p_device->CreateRenderTargetView(back_buffer_ptr, NULL, &p_render_target_view))) {
        GXLOGE("Failed to create render target view.")
    }
    back_buffer_ptr->Release();
    back_buffer_ptr = nullptr;

    setz(depth_buffer_desc);
    depth_buffer_desc.Width = sysapp->get_width();
    depth_buffer_desc.Height = sysapp->get_height();
    depth_buffer_desc.MipLevels = 1;
    depth_buffer_desc.ArraySize = 1;
    depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buffer_desc.SampleDesc.Count = 1;
    depth_buffer_desc.SampleDesc.Quality = 0;
    depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_desc.CPUAccessFlags = 0;
    depth_buffer_desc.MiscFlags = 0;
    GXLOGE("TODO I like 32bit depth buffer.");
    if (FAILED(p_device->CreateTexture2D(&depth_buffer_desc, NULL, &p_depth_stencil_buffer))) {
        GXLOGF("Can not create texture 2d of depth stencil buffer.");
    }
    setz(depth_stencil_desc);
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
    if (FAILED(p_device->CreateDepthStencilState(&depth_stencil_desc, &p_depth_stencil_state))) {
        GXLOGF("Failed to create depth stencil state");
    }
    p_immediate_context->OMSetDepthStencilState(p_depth_stencil_state, 1);
    setz(depth_stencil_view_desc);
    depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;
    if (FAILED(p_device->CreateDepthStencilView(p_depth_stencil_buffer, &depth_stencil_view_desc, &p_depth_stencil_view))) {
        GXLOGF("Failed to create depth stencil view.");
    }
    p_immediate_context->OMSetRenderTargets(1, &p_render_target_view, p_depth_stencil_view);
    setz(raster_desc);
    raster_desc.AntialiasedLineEnable = false;
    raster_desc.CullMode = D3D11_CULL_BACK;
    raster_desc.DepthBias = 0;
    raster_desc.DepthBiasClamp = 0.0f;
    raster_desc.DepthClipEnable = true;
    raster_desc.FillMode = D3D11_FILL_SOLID;
    raster_desc.FrontCounterClockwise = false;
    raster_desc.MultisampleEnable = false;
    raster_desc.ScissorEnable = false;
    raster_desc.SlopeScaledDepthBias = 0.0f;
    GXLOGE("TODO in future support Antialiasing, Multisampling");
    if (FAILED(p_device->CreateRasterizerState(&raster_desc, &p_raster_state))) {
        GXLOGF("Failed to create raster state.");
    }
    p_immediate_context->RSSetState(p_raster_state);
    viewport.Width = (float)sysapp->get_width();
    viewport.Height = (float)sysapp->get_height();
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    p_immediate_context->RSSetViewports(1, &viewport);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    const VertexType vertices[3] = {
        { { -1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { { 1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    };
    const std::uint32_t indices[3] = { 0, 1, 2 };
    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;

    setz(vertex_buffer_desc);
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(vertices);
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    setz(vertex_data);
    vertex_data.pSysMem = vertices;

    if (FAILED(p_device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &p_vertex_buffer))) {
        GXLOGF("Failed to create vertex buffer.");
    }

    setz(index_buffer_desc);
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = sizeof(indices);
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    setz(index_data);
    index_data.pSysMem = indices;

    if (FAILED(p_device->CreateBuffer(&index_buffer_desc, &index_data, &p_index_buffer))) {
        GXLOGF("Failed to create index buffer.");
    }

    ID3DBlob* error_message = nullptr;
    ID3DBlob* vertex_shader_code = nullptr;
    ID3DBlob* fragment_shader_code = nullptr;
    D3D11_INPUT_ELEMENT_DESC polygon_layout[2];

    const char p_vertex_src_data[] = "cbuffer UniformBuffer {\n"
                                     "    matrix mvp;\n"
                                     "};\n"
                                     "struct VertexInputType {\n"
                                     "    float3 position : POSITION;\n"
                                     "    float3 color : COLOR;\n"
                                     "};\n"
                                     "struct PixelInputType\n"
                                     "{\n"
                                     "    float4 position : SV_POSITION;\n"
                                     "    float3 color : COLOR;\n"
                                     "};\n"
                                     "PixelInputType main(VertexInputType input) {\n"
                                     "    PixelInputType output;\n"
                                     "    output.position = mul(float4(input.position, 1.0), mvp);\n"
                                     "    output.color = input.color;\n"
                                     "    return output;\n"
                                     "}\n";
    const char p_fragment_src_data[] = "struct PixelInputType {\n"
                                       "    float4 position : SV_POSITION;\n"
                                       "    float3 color : COLOR;\n"
                                       "};\n"
                                       "float4 main(PixelInputType input) : SV_TARGET {\n"
                                       "	return float4(input.color, 1.0);\n"
                                       "}\n";

    if (FAILED(D3DCompile2(
            p_vertex_src_data,
            sizeof(p_vertex_src_data),
            nullptr,
            nullptr,
            nullptr,
            "main",
            "vs_5_0",
            D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
            0,
            0,
            nullptr,
            0,
            &vertex_shader_code,
            &error_message))) {
        GXLOGF("Error in compiling shader " << ((const char*)(error_message->GetBufferPointer())));
    }

    error_message = nullptr;
    if (FAILED(D3DCompile2(
            p_fragment_src_data,
            sizeof(p_fragment_src_data),
            nullptr,
            nullptr,
            nullptr,
            "main",
            "ps_5_0",
            D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
            0,
            0,
            nullptr,
            0,
            &fragment_shader_code,
            &error_message))) {
        GXLOGF("Error in compiling shader " << ((const char*)(error_message->GetBufferPointer())));
    }
    if (FAILED(p_device->CreateVertexShader(vertex_shader_code->GetBufferPointer(), vertex_shader_code->GetBufferSize(), NULL, &p_vertex_shader))) {
        UNEXPECTED;
    }
    if (FAILED(p_device->CreatePixelShader(fragment_shader_code->GetBufferPointer(), fragment_shader_code->GetBufferSize(), NULL, &p_fragment_shader))) {
        UNEXPECTED;
    }

    setz(polygon_layout[0]);
    polygon_layout[0].SemanticName = "POSITION";
    polygon_layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygon_layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    setz(polygon_layout[1]);
    polygon_layout[1].SemanticName = "COLOR";
    polygon_layout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygon_layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygon_layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    if (FAILED(p_device->CreateInputLayout(
            polygon_layout,
            countof(polygon_layout),
            vertex_shader_code->GetBufferPointer(),
            vertex_shader_code->GetBufferSize(),
            &p_input_layout))) {
        UNEXPECTED;
    }

    vertex_shader_code->Release();
    vertex_shader_code = nullptr;

    fragment_shader_code->Release();
    fragment_shader_code = nullptr;

    D3D11_BUFFER_DESC uniform_buffer_desc;
    setz(uniform_buffer_desc);
    uniform_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    uniform_buffer_desc.ByteWidth = sizeof(UniformBufferType);
    uniform_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    uniform_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    if (FAILED(p_device->CreateBuffer(&uniform_buffer_desc, NULL, &p_uniform_buffer))) {
        UNEXPECTED;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    p_immediate_context->ClearRenderTargetView(p_render_target_view, clear_color);
    p_immediate_context->ClearDepthStencilView(p_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
    // scene render in here
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    const unsigned int stride = sizeof(VertexType);
    const unsigned int offset = 0;
    p_immediate_context->IASetVertexBuffers(0, 1, &p_vertex_buffer, &stride, &offset);
    p_immediate_context->IASetIndexBuffer(p_index_buffer, DXGI_FORMAT_R32_UINT, 0);
    p_immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    if (FAILED(p_immediate_context->Map(p_uniform_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource))) {
        UNEXPECTED;
    }
    UniformBufferType* data_ptr = (UniformBufferType*)mapped_resource.pData;
    data_ptr->mvp = math::Mat4x4::perspective(2.0f, 1.0f, 1.0f, 10.0f) * 
		math::Mat4x4::look_at(
			math::Vec3(0.0f, 0.0f, 4.0f),
			math::Vec3(0.0f, 0.0f, 0.0f),
			math::Vec3(0.0f, 1.0f, 0.0f))
        * math::Mat4x4();
    p_immediate_context->Unmap(p_uniform_buffer, 0);
    unsigned int buffer_number = 0;
    p_immediate_context->VSSetConstantBuffers(buffer_number, 1, &p_uniform_buffer);
    p_immediate_context->IASetInputLayout(p_input_layout);
    p_immediate_context->VSSetShader(p_vertex_shader, NULL, 0);
    p_immediate_context->PSSetShader(p_fragment_shader, NULL, 0);
    p_immediate_context->DrawIndexed(3, 0, 0);
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    p_swapchain->Present(1, 0);
}

void gearoenix::dx11::Engine::terminate()
{
    if (p_swapchain == nullptr)
        return;
    p_swapchain->SetFullscreenState(false, NULL);
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    p_uniform_buffer->Release();
    p_uniform_buffer = nullptr;
    p_input_layout->Release();
    p_input_layout = 0;
    p_fragment_shader->Release();
    p_fragment_shader = nullptr;
    p_vertex_shader->Release();
    p_vertex_shader = nullptr;
    p_index_buffer->Release();
    p_index_buffer = nullptr;
    p_vertex_buffer->Release();
    p_vertex_buffer = nullptr;
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    p_raster_state->Release();
    p_raster_state = nullptr;
    p_depth_stencil_view->Release();
    p_depth_stencil_view = nullptr;
    p_depth_stencil_state->Release();
    p_depth_stencil_state = nullptr;
    p_depth_stencil_buffer->Release();
    p_depth_stencil_buffer = nullptr;
    p_render_target_view->Release();
    p_render_target_view = nullptr;
    p_immediate_context->Release();
    p_immediate_context = nullptr;
    p_device->Release();
    p_device = nullptr;
    p_swapchain->Release();
    p_swapchain = nullptr;
}

gearoenix::render::texture::Texture2D* gearoenix::dx11::Engine::create_texture_2d(system::File* file, std::shared_ptr<core::EndCaller> c)
{
    UNIMPLEMENTED;
    return nullptr;
}

gearoenix::render::texture::Cube* gearoenix::dx11::Engine::create_texture_cube(system::File* file, std::shared_ptr<core::EndCaller> c)
{
    UNIMPLEMENTED;
    return nullptr;
}

gearoenix::render::buffer::Mesh* gearoenix::dx11::Engine::create_mesh(unsigned int vec, system::File* file, std::shared_ptr<core::EndCaller> c)
{
    UNIMPLEMENTED;
    return nullptr;
}

gearoenix::render::buffer::Uniform* gearoenix::dx11::Engine::create_uniform(unsigned int s, std::shared_ptr<core::EndCaller> c)
{
    UNIMPLEMENTED;
    return nullptr;
}

gearoenix::render::shader::Shader* gearoenix::dx11::Engine::create_shader(core::Id sid, system::File* file, std::shared_ptr<core::EndCaller> c)
{
    UNIMPLEMENTED;
    return nullptr;
}

gearoenix::render::shader::Resources* gearoenix::dx11::Engine::create_shader_resources(core::Id sid, render::pipeline::Pipeline* p, render::buffer::Uniform* ub, std::shared_ptr<core::EndCaller> c)
{
    UNIMPLEMENTED;
    return nullptr;
}

gearoenix::render::pipeline::Pipeline* gearoenix::dx11::Engine::create_pipeline(core::Id sid, std::shared_ptr<core::EndCaller> c)
{
    UNIMPLEMENTED;
    return nullptr;
}

ID3D11Device * gearoenix::dx11::Engine::get_device()
{
	return p_device;
}

const ID3D11Device * gearoenix::dx11::Engine::get_device() const
{
	return p_device;
}


ID3D11DeviceContext* gearoenix::dx11::Engine::get_context()
{
	return p_immediate_context;
}

const ID3D11DeviceContext* gearoenix::dx11::Engine::get_context() const
{
	return p_immediate_context;
}

#endif