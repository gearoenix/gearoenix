#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "gx-dxr-loader.hpp"

namespace gearoenix::dxr {
struct Device;
struct Command final {
private:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> list;
    bool is_recording = false;

public:
    Command(Device& device, D3D12_COMMAND_LIST_TYPE type) noexcept;
    ~Command() noexcept;
    Command(Command&&) noexcept = default;
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;

    [[nodiscard]] ID3D12GraphicsCommandList6* get_list() noexcept;
    void begin(ID3D12PipelineState* const pipeline_state) noexcept;
    void close() noexcept;
    void force_close() noexcept;
};
}

#endif