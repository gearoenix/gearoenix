#include "gx-vk-des-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-des-bindings-data.hpp"
#include "gx-vk-des-pool.hpp"
#include "gx-vk-des-set.hpp"
#include <cstring>

gearoenix::vulkan::descriptor::Manager::Manager(const device::Logical& logical_device)
    : imgui(Pool::create_imgui(logical_device))
    , logical_device(logical_device)
{
}

gearoenix::vulkan::descriptor::Manager::~Manager() = default;

std::unique_ptr<gearoenix::vulkan::descriptor::BindingsData>& gearoenix::vulkan::descriptor::Manager::get_bindings_data(
    const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
    const auto bsz = bindings.size() * sizeof(bindings[0]);
    std::vector<std::uint8_t> key(bsz);
    std::memcpy(key.data(), bindings.data(), bsz);
    return bindings_data[key];
}

std::shared_ptr<gearoenix::vulkan::descriptor::Set> gearoenix::vulkan::descriptor::Manager::create_set(
    const std::vector<VkDescriptorSetLayoutBinding>& bindings,
    const std::optional<std::uint64_t> kernel_index)
{
    auto& bd = get_bindings_data(bindings);
    if (nullptr == bd) {
        bd = std::unique_ptr<BindingsData>(new BindingsData(logical_device, bindings));
    }
    return bd->create_set(kernel_index);
}

#endif
