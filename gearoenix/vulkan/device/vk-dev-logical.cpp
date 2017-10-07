#include "vk-dev-logical.hpp"
#include "../../core/cr-static.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"
#include "vk-dev-physical.hpp"

gearoenix::render::device::Logical::Logical(Physical* p)
    : physical_device(p)
{
    const char* device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    auto l = physical_device->get_instance()->get_linker();
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    float queue_priorities[] = { 1.0f };
    // TODO: create as many as possible queue to separate independent works as
    // many as possible
    // on the queues but it is not required currently
    std::set<uint32_t> queue_index_set;
    queue_index_set.insert(p->get_graphics_queue_node_index());
    queue_index_set.insert(p->get_transfer_queue_node_index());
    queue_index_set.insert(p->get_compute_queue_node_index());
    queue_index_set.insert(p->get_present_queue_node_index());
    for (auto q : queue_index_set) {
        if (q == UINT32_MAX)
            continue;
        VkDeviceQueueCreateInfo queue_create_info;
        setz(queue_create_info);
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueCount = 1;
        queue_create_info.queueFamilyIndex = q;
        queue_create_info.pQueuePriorities = queue_priorities;
        queue_create_infos.push_back(queue_create_info);
        //		LOGE(std::string("queue node index added is ") +
        // std::to_string(int(q)));
    }
    VkPhysicalDeviceFeatures device_features;
    setz(device_features);
    device_features.samplerAnisotropy = VK_TRUE;
    VkDeviceCreateInfo device_create_info;
    setz(device_create_info);
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
    device_create_info.enabledExtensionCount = countof(device_extensions);
    device_create_info.ppEnabledExtensionNames = device_extensions;
    device_create_info.pEnabledFeatures = &device_features;
    VKC(l->vkCreateDevice(physical_device->get_vulkan_data(), &device_create_info,
        0, &vulkan_data));
    l->vkGetDeviceQueue(vulkan_data,
        physical_device->get_graphics_queue_node_index(), 0,
        &vk_graphic_queue);
}

gearoenix::render::device::Logical::~Logical()
{
    auto l = physical_device->get_instance()->get_linker();
    l->vkDestroyDevice(vulkan_data, nullptr);
}

const gearoenix::render::device::Physical* gearoenix::render::device::Logical::get_physical_device() const
{
    return physical_device;
}
gearoenix::render::device::Physical* gearoenix::render::device::Logical::get_physical_device()
{
    return physical_device;
}

const VkDevice& gearoenix::render::device::Logical::get_vulkan_data() const
{
    return vulkan_data;
}

const VkQueue& gearoenix::render::device::Logical::get_graphic_queue() const
{
    return vk_graphic_queue;
}

void gearoenix::render::device::Logical::wait_to_finish()
{
    auto l = physical_device->get_instance()->get_linker();
    l->vkDeviceWaitIdle(vulkan_data);
}
