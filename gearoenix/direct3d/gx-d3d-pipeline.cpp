#include "gx-d3d-pipeline.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/stream/gx-plt-stm-asset.hpp"
#include "gx-d3d-check.hpp"
#include "gx-d3d-device.hpp"
#include "gx-d3d-engine.hpp"
#include <fstream>

gearoenix::d3d::PipelineManager::PipelineManager(Engine& e)
    : e(e)
    , device(e.get_device())
{
    initialize_g_buffer_filler();
    initialize_mipmap_generator();
}

gearoenix::d3d::PipelineManager::~PipelineManager() = default;

void gearoenix::d3d::PipelineManager::initialize_g_buffer_filler()
{
    const std::unique_ptr<platform::stream::Asset> g_buffer_vs_f(platform::stream::Asset::construct(e.get_platform_application(), "shaders/gx-d3d-shd-gbuffers-filler.vs.cso"));
    GX_ASSERT(nullptr != g_buffer_vs_f);
    const auto g_buffer_vs_f_shader_bin = g_buffer_vs_f->get_file_content();

    const std::unique_ptr<platform::stream::Asset> g_buffer_ps_f(platform::stream::Asset::construct(e.get_platform_application(), "shaders/gx-d3d-shd-gbuffers-filler.ps.cso"));
    GX_ASSERT(nullptr != g_buffer_ps_f);
    const auto g_buffer_ps_f_shader_bin = g_buffer_ps_f->get_file_content();

    auto* const d = device->get_device().Get();

    GX_D3D_CHECK(d->CreateRootSignature(0, g_buffer_vs_f_shader_bin.data(), g_buffer_vs_f_shader_bin.size(), IID_PPV_ARGS(&g_buffer_filler_root_signature)));

    const D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
        D3D12_INPUT_ELEMENT_DESC {
            .SemanticName = "POSITION",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        },
        D3D12_INPUT_ELEMENT_DESC {
            .SemanticName = "NORMAL",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 12,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        },
        D3D12_INPUT_ELEMENT_DESC {
            .SemanticName = "TANGENT",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 24,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        },
        D3D12_INPUT_ELEMENT_DESC {
            .SemanticName = "TEXCOORD",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 40,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        },
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc;
    GX_SET_ZERO(pso_desc);
    pso_desc.InputLayout.pInputElementDescs = input_element_descs;
    pso_desc.InputLayout.NumElements = GX_COUNT_OF(input_element_descs);
    pso_desc.pRootSignature = g_buffer_filler_root_signature.Get();
    pso_desc.VS.pShaderBytecode = g_buffer_vs_f_shader_bin.data();
    pso_desc.VS.BytecodeLength = g_buffer_vs_f_shader_bin.size();
    pso_desc.PS.pShaderBytecode = g_buffer_ps_f_shader_bin.data();
    pso_desc.PS.BytecodeLength = g_buffer_ps_f_shader_bin.size();
    pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pso_desc.SampleMask = UINT_MAX;
    pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso_desc.NumRenderTargets = 1;
    pso_desc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pso_desc.SampleDesc.Count = 1;
    GX_D3D_CHECK(d->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&g_buffer_filler_pipeline_state)));
}

void gearoenix::d3d::PipelineManager::initialize_mipmap_generator()
{
    const std::unique_ptr<platform::stream::Asset> f(platform::stream::Asset::construct(e.get_platform_application(), "shaders/gx-d3d-shd-mipmap-generator.cs.cso"));
    GX_ASSERT(nullptr != f);
    const auto b = f->get_file_content();

    auto* const d = device->get_device().Get();

    GX_D3D_CHECK(d->CreateRootSignature(0, b.data(), b.size(), IID_PPV_ARGS(&mipmap_generator_root_signature)));

    D3D12_COMPUTE_PIPELINE_STATE_DESC pso_desc;
    GX_SET_ZERO(pso_desc);
    pso_desc.pRootSignature = mipmap_generator_root_signature.Get();
    pso_desc.CS.pShaderBytecode = b.data();
    pso_desc.CS.BytecodeLength = b.size();
    GX_D3D_CHECK(d->CreateComputePipelineState(&pso_desc, IID_PPV_ARGS(&mipmap_generator_pipeline_state)));
}

#endif