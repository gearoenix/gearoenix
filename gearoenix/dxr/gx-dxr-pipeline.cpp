#include "gx-dxr-pipeline.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-device.hpp"
#include <fstream>

gearoenix::dxr::PipelineManager::PipelineManager(std::shared_ptr<Device> _device) noexcept
    : device(std::move(_device))
{
    std::fstream vf("../../../../assets/gx-dxr-shd-g-buffer.vs.cso", std::ios::binary | std::ios::in);
    GX_ASSERT_D(vf.is_open() && vf.good());
    vf.seekg(0, std::ios::end);
    std::vector<char> vsb(vf.tellg());
    vf.seekg(0);
    vf.read(vsb.data(), vsb.size());

    std::fstream pf("../../../../assets/gx-dxr-shd-g-buffer.ps.cso", std::ios::binary | std::ios::in);
    GX_ASSERT_D(pf.is_open() && pf.good());
    pf.seekg(0, std::ios::end);
    std::vector<char> psb(pf.tellg());
    pf.seekg(0);
    pf.read(psb.data(), psb.size());

    auto* const d = device->get_device().Get();

    GX_DXR_CHECK(d->CreateRootSignature(0, vsb.data(), vsb.size(), IID_PPV_ARGS(&g_buffer_filler_root_signature)))

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
            .AlignedByteOffset = 24,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        },
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc;
    GX_SET_ZERO(pso_desc)
    pso_desc.InputLayout.pInputElementDescs = input_element_descs;
    pso_desc.InputLayout.NumElements = GX_COUNT_OF(input_element_descs);
    pso_desc.pRootSignature = g_buffer_filler_root_signature.Get();
    pso_desc.VS.pShaderBytecode = vsb.data();
    pso_desc.VS.BytecodeLength = vsb.size();
    pso_desc.PS.pShaderBytecode = psb.data();
    pso_desc.PS.BytecodeLength = psb.size();
    pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso_desc.DepthStencilState.DepthEnable = FALSE;
    pso_desc.DepthStencilState.StencilEnable = FALSE;
    pso_desc.SampleMask = UINT_MAX;
    pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso_desc.NumRenderTargets = 1;
    pso_desc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    pso_desc.SampleDesc.Count = 1;
    GX_DXR_CHECK(d->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&g_buffer_filler_pipeline_state)))
}

gearoenix::dxr::PipelineManager::~PipelineManager() noexcept = default;

#endif