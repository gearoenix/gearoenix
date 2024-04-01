#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "gx-d3d-loader.hpp"

namespace gearoenix::d3d {
struct Device;
struct Command final {
private:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> list;
    bool is_recording = false;

public:
    Command(Device& device, D3D12_COMMAND_LIST_TYPE type);
    ~Command();
    Command(Command&&) = default;
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;

    [[nodiscard]] ID3D12GraphicsCommandList6* get_list();
    void begin(ID3D12PipelineState* const pipeline_state);
    void close();
    void force_close();
};
}

#endif