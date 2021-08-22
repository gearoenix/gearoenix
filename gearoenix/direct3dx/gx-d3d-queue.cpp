#include "gx-d3d-queue.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "gx-d3d-check.hpp"
#include "gx-d3d-device.hpp"

gearoenix::direct3dx::Queue::Queue(std::shared_ptr<Device> _device) noexcept
    : device(std::move(_device))
{
    D3D12_COMMAND_QUEUE_DESC queue_desc;
    GX_SET_ZERO(queue_desc)
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    GX_D3D_CHECK(device->get_device()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue)))
}

#endif
