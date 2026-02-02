#include "gx-vk-qu-queue.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../sync/gx-vk-sync-fence.hpp"

#include <boost/container/flat_map.hpp>
#include <mutex>
#include <thread>

gearoenix::vulkan::queue::Queue::Queue()
    : Singleton(this)
    , vulkan_data([&] {
        VkQueue q = nullptr;
        vkGetDeviceQueue(device::Logical::get().get_vulkan_data(), device::Physical::get().get_graphics_queue_node_index(), 0, &q);
        return q;
    }())
{
}

gearoenix::vulkan::queue::Queue::~Queue() = default;

void gearoenix::vulkan::queue::Queue::submit(const command::Buffer& cmd, const sync::Fence& fence)
{
    VkSubmitInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.commandBufferCount = 1;
    info.pCommandBuffers = cmd.get_vulkan_data_ptr();
    GX_VK_CHK(vkQueueSubmit(vulkan_data, 1, &info, fence.get_vulkan_data()));
}

void gearoenix::vulkan::queue::Queue::submit(
    const std::uint64_t wait_semaphores_count,
    const VkSemaphore* const wait_semaphores,
    const VkPipelineStageFlags* const wait_stages,
    const std::uint64_t commands_count,
    const VkCommandBuffer* const commands,
    const std::uint64_t signal_semaphores_count,
    const VkSemaphore* const signal_semaphores,
    const VkFence fence)
{
    VkSubmitInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = static_cast<std::uint32_t>(wait_semaphores_count);
    info.pWaitSemaphores = wait_semaphores;
    info.pWaitDstStageMask = wait_stages;
    info.commandBufferCount = static_cast<std::uint32_t>(commands_count);
    info.pCommandBuffers = commands;
    info.signalSemaphoreCount = static_cast<std::uint32_t>(signal_semaphores_count);
    info.pSignalSemaphores = signal_semaphores;
    GX_VK_CHK(vkQueueSubmit(vulkan_data, 1, &info, fence));
}

#endif
