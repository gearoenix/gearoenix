#include "gx-d3d-queue.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "gx-d3d-check.hpp"
#include "gx-d3d-device.hpp"

gearoenix::d3d::Queue::Queue(std::shared_ptr<Device> _device, const Type ty)
    : device(std::move(_device))
{
    D3D12_COMMAND_QUEUE_DESC queue_desc;
    GX_SET_ZERO(queue_desc);
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    switch (ty) {
    case Type::Compute:
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        break;
    case Type::Copy:
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        break;
    case Type::Direct:
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        break;
    }
    GX_D3D_CHECK(device->get_device()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue)));
}

gearoenix::d3d::Queue::~Queue() = default;

void gearoenix::d3d::Queue::exe(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6>& cmd)
{
    ID3D12CommandList* cmds[] = { cmd.Get() };
    command_queue->ExecuteCommandLists(1, cmds);
}

void gearoenix::d3d::Queue::exe(ID3D12CommandList** cmds, UINT count)
{
    command_queue->ExecuteCommandLists(count, cmds);
}
#endif
