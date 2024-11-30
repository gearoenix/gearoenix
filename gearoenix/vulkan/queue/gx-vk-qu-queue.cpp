#include "gx-vk-qu-queue.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"
#include <boost/container/flat_map.hpp>
#include <mutex>
#include <thread>

gearoenix::vulkan::queue::Queue::Queue(engine::Engine& e)
    : e(e)
{
    static std::mutex queue_index_lock;
    static std::uint32_t latest_queue_index = 0;
    static boost::container::flat_map<std::thread::id, std::uint32_t> thread_queue_index_map {};
    std::lock_guard<std::mutex> _lg(queue_index_lock);
    uint32_t queue_index = 0;
    if (auto queue_index_search = thread_queue_index_map.find(std::this_thread::get_id()); thread_queue_index_map.end() != queue_index_search) {
        queue_index = queue_index_search->second;
    } else {
        queue_index = latest_queue_index;
        ++latest_queue_index;
        thread_queue_index_map.emplace(std::this_thread::get_id(), queue_index);
    }
    vkGetDeviceQueue(
        e.get_logical_device().get_vulkan_data(),
        e.get_physical_device().get_graphics_queue_node_index(),
        queue_index,
        &vulkan_data);
}

gearoenix::vulkan::queue::Queue::~Queue() = default;

void gearoenix::vulkan::queue::Queue::submit(command::Buffer& cmd, sync::Fence& fence)
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
    VkFence fence)
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
