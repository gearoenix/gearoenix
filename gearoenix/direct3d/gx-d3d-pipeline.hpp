#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "gx-d3d-loader.hpp"

namespace gearoenix::d3d {
struct Device;
struct Engine;
struct PipelineManager final {
    GX_GET_RRF_PRV(Engine, e)
    GX_GET_REFC_PRV(std::shared_ptr<Device>, device)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12RootSignature>, g_buffer_filler_root_signature)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12PipelineState>, g_buffer_filler_pipeline_state)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12RootSignature>, mipmap_generator_root_signature)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12PipelineState>, mipmap_generator_pipeline_state)

    PipelineManager(Engine& e);
    ~PipelineManager();

private:
    void initialize_g_buffer_filler();
    void initialize_mipmap_generator();
};
}

#endif