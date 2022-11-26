#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/sync/gx-cr-sync-end-caller.hpp"
#include "../core/sync/gx-cr-sync-work-waiter.hpp"
#include "gx-d3d-loader.hpp"

#include <memory>

namespace gearoenix::d3d {

struct Device;
struct GpuBuffer;
struct Queue;
struct Texture2D;

struct Uploader final {
    GX_GET_REFC_PRV(std::shared_ptr<Device>, device)

private:
    core::sync::WorkWaiter uploader;
    std::shared_ptr<Queue> copy_queue;
    std::shared_ptr<Queue> direct_queue;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    UINT64 fence_value = 1;

    void wait(UINT64 fv) noexcept;

public:
    explicit Uploader(std::shared_ptr<Device> device) noexcept;
    ~Uploader() noexcept;
    void upload(std::vector<std::uint8_t>&& data, std::shared_ptr<GpuBuffer>&& buffer, core::sync::EndCaller&& end) noexcept;
    void upload(std::vector<std::uint8_t>&& data, std::shared_ptr<Texture2D>&& texture, UINT subresource, core::sync::EndCaller&& end) noexcept;
};
}

#endif